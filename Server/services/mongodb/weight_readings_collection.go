package mongodb

import (
	"Server/customerrors"
	"Server/models"
	"context"
	"go.mongodb.org/mongo-driver/mongo"
)

const weightReadingCollectionName = "weight_reading"

var weightReadingsCollection *mongo.Collection

func init() {
	weightReadingsCollection = database.Collection(weightReadingCollectionName)
}

func InsertWeightReading(reading models.Reading) error {
	_, err := weightReadingsCollection.InsertOne(context.TODO(), reading)
	if err == nil {
		return nil
	}

	return customerrors.Wrapf(err, "error inserting weight reading for device %s", reading.Device)
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

func DeleteWeightReadingList(filter interface{}) (int64, error) {
	result, err := weightReadingsCollection.DeleteMany(context.TODO(), filter)
	if err == nil {
		return result.DeletedCount, nil
	}

	return 0, customerrors.Wrap(err, "error deleting weight reading list")
}