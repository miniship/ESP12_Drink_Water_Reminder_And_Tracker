package mongodb

import (
	"Server/customerrors"
	"Server/models"
	"context"
	"fmt"
	"go.mongodb.org/mongo-driver/mongo"
)

const WeightReadingCollection = "weight_reading"

var weightReadingsCollection *mongo.Collection

func init() {
	weightReadingsCollection = GetMongoDatabase().Collection(WeightReadingCollection)
}

func InsertWeightReading(reading models.Reading) error {
	_, err := weightReadingsCollection.InsertOne(context.TODO(), reading)
	if err == nil {
		return nil
	}

	msg := fmt.Sprintf("error inserting weight reading for device %s", reading.Device)
	return customerrors.Wrap(err, msg)
}

func FindWeightReadingList(filter interface{}) ([]models.Reading, error) {
	var readingList []models.Reading
	cursor, err := weightReadingsCollection.Find(context.TODO(), filter)
	if err != nil {
		return readingList, customerrors.Wrap(err, "error finding weight reading list, query step")
	}

	err = cursor.All(context.TODO(), &readingList)
	if err != nil {
		return readingList, customerrors.Wrap(err, "error finding weight reading list, decode step")
	}

	return readingList, nil
}