package interactor

import (
	"Server/customerrors"
	"Server/models"
	"Server/services/jwtService"
	"Server/services/mongodb"
	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"golang.org/x/crypto/bcrypt"
	"log"
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

func UpdateUserDevices(username string, deviceList []string) error {
	filter := bson.M{"username":username}
	update := bson.M{"$set":bson.M{"deviceList":deviceList}}

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
	return mongodb.DeleteUser(bson.M{"username":username})
}

func UserLogin(loginUser models.User) (string, error) {
	foundUser, err := mongodb.FindUser(bson.M{"username":loginUser.Username})

	if err != nil {
		if err == mongo.ErrNoDocuments {
			return "", customerrors.NotFound.New("user not found")
		}
		return  "", err
	}

	if !comparePasswords(foundUser.Password, []byte(loginUser.Password)) {
		return "", customerrors.BadRequest.New("invalid username or password")
	}

	jwt, err := jwtService.GenerateJwt(loginUser)
	if err != nil {
		return "", customerrors.New("generate jwt failed")
	}

	return jwt, nil
}

func hashAndSalt(pwd []byte) (string, error) {
	hash, err := bcrypt.GenerateFromPassword(pwd, bcrypt.MinCost)
	if err != nil {
		return string(pwd), err
	}
	return string(hash), nil
}

func comparePasswords(hashedPwd string, plainPwd []byte) bool {
	byteHash := []byte(hashedPwd)
	err := bcrypt.CompareHashAndPassword(byteHash, plainPwd)
	if err != nil {
		log.Println("[comparePasswords]", err)
		return false
	}

	return true
}