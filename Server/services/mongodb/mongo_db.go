package mongodb

import (
	"context"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"go.mongodb.org/mongo-driver/mongo/readpref"
	"log"
	"time"
)

const ConnectionString = "mongodb://localhost:27017"
const Database = "water-reminder"

func GetMongoDatabase() *mongo.Database {
	ctx, _ := context.WithTimeout(context.Background(), 10 * time.Second)
	clientOptions := options.Client().ApplyURI(ConnectionString)

	client, err := mongo.Connect(ctx, clientOptions)
	if err != nil {
		log.Fatal("[GetMongoDatabase]", err)
	}

	ctx, _ = context.WithTimeout(context.Background(), 2 * time.Second)
	err = client.Ping(ctx, readpref.Primary())

	if err != nil {
		log.Fatal("[GetMongoDatabase]", err)
	}

	log.Println("[GetMongoDatabase] connected to database: ", Database)
	return client.Database(Database)
}