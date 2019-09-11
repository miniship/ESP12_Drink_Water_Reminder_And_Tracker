package database

import (
	"context"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"go.mongodb.org/mongo-driver/mongo/readpref"
	"log"
	"time"
)

const CONNECTION_STRING = "mongodb://localhost:27017"
const DB_NAME = "water-reminder"

func GetMongoDatabase() *mongo.Database {
	ctx, _ := context.WithTimeout(context.Background(), 10 * time.Second)
	clientOptions := options.Client().ApplyURI(CONNECTION_STRING)

	client, err := mongo.Connect(ctx, clientOptions)
	if err != nil {
		log.Fatal(err)
	}

	ctx, _ = context.WithTimeout(context.Background(), 2 * time.Second)
	err = client.Ping(ctx, readpref.Primary())

	if err != nil {
		log.Fatal(err)
	}

	log.Println("Connected to database: ", DB_NAME)
	return client.Database(DB_NAME)
}