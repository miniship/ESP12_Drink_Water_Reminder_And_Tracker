package interactors

import (
	"Server/customerrors"
	"Server/models"
	"Server/services/mongodb"
	"Server/services/mqtt"
	"go.mongodb.org/mongo-driver/bson"
	"golang.org/x/crypto/bcrypt"
)

func RegisterUser(user models.User) error {
	// check duplicate username
	_, err := mongodb.FindUser(bson.M{"username":user.Username})
	if err == nil {
		return customerrors.BadRequest.New("username existed")
	}

	// server error
	if customerrors.GetType(err) != customerrors.NotFound {
		return err
	}

	// username not existed yet, insert new user
	passwordHash, err := hashAndSalt([]byte(user.Password))
	if err != nil {
		return err
	}

	user.Password = passwordHash
	if err := mongodb.InsertUser(user); err != nil {
		return err
	}

	return nil
}

func hashAndSalt(pwd []byte) (string, error) {
	hash, err := bcrypt.GenerateFromPassword(pwd, bcrypt.MinCost)
	if err != nil {
		return string(pwd), customerrors.Wrap(err, "error generating password hash")
	}

	return string(hash), nil
}

func ListAllUser() ([]models.User, error) {
	return mongodb.FindUserList(bson.D{})
}

func DeleteUserByName(username string) error {
	filter := bson.M{"username":username}
	user, err := mongodb.FindUser(filter)
	if err != nil {
		return err
	}

	if err := mongodb.DeleteUser(filter); err != nil {
		return err
	}

	deviceList := user.DeviceList
	if _, err := mongodb.DeleteWeightReadingList(bson.M{"device":bson.M{"$in":deviceList}}); err != nil {
		return err
	}

	for _, device := range deviceList {
		if err := mqtt.UnsubscribeForDevice(device); err != nil {
			return err
		}
	}

	return nil
}

func ListUserDevices(username string) ([]string, error) {
	user, err := mongodb.FindUser(bson.M{"username":username})
	if err != nil {
		return nil, err
	}

	return user.DeviceList, nil
}

func AddUserDevice(username string, device string) error {
	if IsDeviceRegistered(device) {
		return customerrors.BadRequest.Newf("error adding device %s for user %s, may be already registered", device, username)
	}

	filter := bson.M{"username":username}
	update := bson.M{"$addToSet":bson.M{"deviceList":device}}
	if err := mongodb.UpdateUser(filter, update); err != nil {
		return err
	}

	if err := mqtt.SubscribeForDevice(device); err != nil {
		return err
	}

	return nil
}

func IsDeviceRegistered(device string) bool {
	filter := bson.M{"deviceList":device}
	if _, err := mongodb.FindUser(filter); err == nil {
		return true
	}

	return false
}

func RemoveUserDevice(username string, device string) error {
	filter := bson.M{"username":username}
	update := bson.M{"$pull":bson.M{"deviceList":device}}

	if err := mongodb.UpdateUser(filter, update); err != nil {
		return err
	}

	if _, err := mongodb.DeleteWeightReadingList(bson.M{"device":device}); err != nil {
		return err
	}

	if err := mqtt.UnsubscribeForDevice(device); err != nil {
		return err
	}

	return nil
}