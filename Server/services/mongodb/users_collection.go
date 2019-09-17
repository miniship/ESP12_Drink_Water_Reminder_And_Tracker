package mongodb

import (
	"Server/customerrors"
	"Server/models"
	"context"
	"fmt"
	"go.mongodb.org/mongo-driver/mongo"
)

const UserCollection = "users"

var usersCollection *mongo.Collection

func init() {
	usersCollection = GetMongoDatabase().Collection(UserCollection)
}

func InsertUser(user models.User) error {
	_, err := usersCollection.InsertOne(context.TODO(), user)
	if err == nil {
		return nil
	}

	msg := fmt.Sprintf("error inserting user %s", user.Username)
	return customerrors.Wrap(err, msg)
}

func FindUser(filter interface{}) (models.User, error) {
	var existedUser models.User
	err := usersCollection.FindOne(context.TODO(), filter).Decode(&existedUser)

	if err == nil {
		return existedUser, nil
	}

	if err == mongo.ErrNoDocuments {
		return existedUser, err
	}

	return existedUser, customerrors.Wrap(err, "error finding user")
}

func FindUserList(filter interface{}) ([]models.User, error) {
	var userList []models.User
	cursor, err := usersCollection.Find(context.TODO(), filter)
	if err != nil {
		return userList, customerrors.Wrap(err, "error finding user list, query step")
	}

	err = cursor.All(context.TODO(), &userList)
	if err != nil {
		return userList, customerrors.Wrap(err, "error finding user list, decode step")
	}

	return userList, nil
}

func UpdateUser(filter interface{}, update interface{}) (int64, int64, error) {
	updateResult, err := usersCollection.UpdateOne(context.TODO(), filter, update)
	if err != nil {
		return 0, 0, customerrors.Wrap(err, "error updating user")
	}

	return updateResult.MatchedCount, updateResult.ModifiedCount, nil
}

func DeleteUser(filter interface{}) (int64, error) {
	result, err := usersCollection.DeleteOne(context.TODO(), filter)
	if err == nil {
		return result.DeletedCount, nil
	}

	return 0, customerrors.Wrap(err, "error deleting user")
}