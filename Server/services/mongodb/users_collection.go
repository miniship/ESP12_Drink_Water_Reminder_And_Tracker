package mongodb

import (
	"Server/customerrors"
	"Server/models"
	"context"
	"go.mongodb.org/mongo-driver/mongo"
)

const userCollectionName = "users"
var usersCollection *mongo.Collection

func init() {
	usersCollection = database.Collection(userCollectionName)
}

func InsertUser(user models.User) error {
	_, err := usersCollection.InsertOne(context.TODO(), user)
	if err == nil {
		return nil
	}

	return customerrors.Wrapf(err, "error inserting user %s", user.Username)
}

func FindUser(filter interface{}) (models.User, error) {
	var existedUser models.User
	err := usersCollection.FindOne(context.TODO(), filter).Decode(&existedUser)

	if err == nil {
		return existedUser, nil
	}

	if err == mongo.ErrNoDocuments {
		return existedUser, customerrors.NotFound.New("user not found")
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

func UpdateUser(filter interface{}, update interface{}) error {
	result, err := usersCollection.UpdateOne(context.TODO(), filter, update)
	if err != nil {
		return customerrors.Wrap(err, "error updating user")
	}

	if result.MatchedCount != 1 || result.ModifiedCount != 1 {
		return customerrors.BadRequest.Newf("expected 1 user is modified but got %d matched user and %d modified user", result.MatchedCount, result.ModifiedCount)
	}

	return nil
}

func DeleteUser(filter interface{}) error {
	result, err := usersCollection.DeleteOne(context.TODO(), filter)
	if err != nil {
		return customerrors.Wrap(err, "error deleting user")
	}

	if result.DeletedCount != 1 {
		return customerrors.BadRequest.Newf("expected 1 matched user but got %d", result.DeletedCount)
	}

	return nil
}