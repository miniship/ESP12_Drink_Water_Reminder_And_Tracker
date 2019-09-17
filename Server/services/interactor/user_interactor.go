package interactor

import (
	"Server/customerrors"
	"Server/models"
	"Server/services/mongodb"
	"Server/services/mqtt"
	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"golang.org/x/crypto/bcrypt"
)

func RegisterUser(user models.User) error {
	// check duplicate username
	_, err := mongodb.FindUser(bson.M{"username":user.Username})
	if err == nil {
		return customerrors.BadRequest.New("username existed")
	}

	// server error
	if err != mongo.ErrNoDocuments {
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
		return string(pwd), err
	}
	return string(hash), nil
}

func ListAllUser() ([]models.User, error) {
	return mongodb.FindUserList(bson.D{})
}

func ListUserDevices(username string) ([]string, error) {
	user, err := mongodb.FindUser(bson.M{"username":username})
	if err != nil {
		return nil, err
	}
	return user.DeviceList, nil
}

func AddUserDevice(username string, device string) error {
	filter := bson.M{"username":username}
	update := bson.M{"$addToSet":bson.M{"deviceList":device}}

	matched, _, err := mongodb.UpdateUser(filter, update)
	if err != nil {
		return err
	}

	if matched == 0 {
		return customerrors.NotFound.New("user not found")
	}

	return nil
}

func RemoveUserDevice(username string, device string) error {
	filter := bson.M{"username":username}
	update := bson.M{"$pull":bson.M{"deviceList":device}}

	matched, _, err := mongodb.UpdateUser(filter, update)
	if err != nil {
		return err
	}

	if matched == 0 {
		return customerrors.NotFound.New("user not found")
	}

	return nil
}

func DeleteUserByName(username string) (int64, error) {
	filter := bson.M{"username":username}
	user, err := mongodb.FindUser(filter)
	if err != nil {
		return 0, err
	}

	for _, device := range user.DeviceList {
		if err := mqtt.UnsubscribeForDevice(device); err != nil {
			return 0, err
		}
	}

	return mongodb.DeleteUser(filter)
}