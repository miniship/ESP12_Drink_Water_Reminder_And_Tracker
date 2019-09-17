package mongodb

import (
	"context"
	"github.com/labstack/gommon/log"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"go.mongodb.org/mongo-driver/mongo/readpref"
	"time"
)

const ConnectionString = "mongodb://localhost:27017"
//const ConnectionString = "mongodb+srv://admin:rr98iZZ6EhiVF9Mq@cluster0-ykwv3.gcp.mongodb.net/water-reminder?retryWrites=true&w=majority"
const Database = "water-reminder"

var database *mongo.Database

func init()  {
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

	log.Info("[GetMongoDatabase] connected to database: ", Database)
	database = client.Database(Database)
}

func GetMongoDatabase() *mongo.Database {
	return database
}