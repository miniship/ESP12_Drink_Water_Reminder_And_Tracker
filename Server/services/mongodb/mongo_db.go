package mongodb

import (
	"context"
	"github.com/labstack/gommon/log"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"go.mongodb.org/mongo-driver/mongo/readpref"
	"time"
)

//const connectionString = "mongodb://localhost:27017"
const connectionString = "mongodb+srv://admin:rr98iZZ6EhiVF9Mq@cluster0-ykwv3.gcp.mongodb.net/water-reminder?retryWrites=true&w=majority"
const databaseName = "water-reminder"

var database *mongo.Database

func init()  {
	ctx, _ := context.WithTimeout(context.Background(), 10 * time.Second)
	clientOptions := options.Client().ApplyURI(connectionString)

	client, err := mongo.Connect(ctx, clientOptions)
	if err != nil {
		log.Error("[MongoDB init]", err)
	}

	ctx, _ = context.WithTimeout(context.Background(), 2 * time.Second)
	err = client.Ping(ctx, readpref.Primary())

	if err != nil {
		log.Error("[MongoDB init]", err)
	}

	log.Info("[MongoDB init] connected to database: ", databaseName)
	database = client.Database(databaseName)
}