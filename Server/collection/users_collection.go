package collection

import (
	"Server/database"
	"Server/model"
	"context"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"log"
	"time"
)
const COLLECTION_NAME = "users"

var c *mongo.Collection

func init() {
	c = database.GetMongoDatabase().Collection(COLLECTION_NAME)
}

func InsertUser(user model.User) bool {
	ctx, _ := context.WithTimeout(context.Background(), 5 * time.Second)
	_, err := c.InsertOne(ctx, user)
	if err != nil {
		log.Println(err)
		return false
	}
	return true
}

// return ErrNoDocuments if not found
func FindUser(filter interface{}) (model.User, error) {
	var exitedUser model.User
	ctx, _ := context.WithTimeout(context.Background(), 5 * time.Second)
	err := c.FindOne(ctx, filter).Decode(&exitedUser)
	return exitedUser, err
}

func UpdateUser(filter interface{}, update interface{}) (int64, int64) {
	ctx, _ := context.WithTimeout(context.Background(), 5 * time.Second)
	updateResult, err := c.UpdateOne(ctx, filter, update)

	if err != nil {
		log.Println(err)
		return updateResult.MatchedCount, updateResult.ModifiedCount
	}

	return updateResult.MatchedCount, updateResult.ModifiedCount
}

func UpsertUser(filter interface{}, update interface{}) (int64, int64) {
	ctx, _ := context.WithTimeout(context.Background(), 5 * time.Second)
	updateResult, err := c.UpdateOne(ctx, filter, update, options.Update().SetUpsert(true))

	if err != nil {
		log.Println(err)
		return updateResult.MatchedCount, updateResult.ModifiedCount
	}

	return updateResult.MatchedCount, updateResult.ModifiedCount
}

func DeleteUser(filter interface{}) bool {
	ctx, _ := context.WithTimeout(context.Background(), 5 * time.Second)
	_, err := c.DeleteOne(ctx, filter)
	if err != nil {
		log.Println(err)
		return false
	}
	return true
}