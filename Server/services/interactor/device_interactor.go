package interactor

import (
	"Server/models"
	"Server/services/mongodb"
	"Server/services/mqtt"
	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"strconv"
)

const CommandTopic = "command/"

func IsDeviceRegistered(device string) (bool, error) {
	filter := bson.M{"deviceList":device}

	_, err := mongodb.FindUser(filter)
	if err == nil {
		return true, nil
	}
	if err == mongo.ErrNoDocuments {
		return false, nil
	}

	return false, err
}

func ListAllWeightReading(device string) ([]models.Reading, error) {
	filter := bson.M{"device":device}
	return mongodb.FindWeightReadingList(filter)
}

func ClearAllWeightReading(device string) (int64, error) {
	filter := bson.M{"device":device}
	return mongodb.DeleteWeightReadingList(filter)
}

func CommandUserDevice(device string, command models.Command) error {
	topic := CommandTopic + device
	code := command.Code
	message := strconv.Itoa(int(code))
	message += ":"
	for _, param := range command.Parameters {
		message += param + ";"
	}

	var qos = 0

	switch code {
	case models.UpdateSchedule:
		qos = 1
		break
	case models.RestartDevice:
	case models.AlertUser:
	default:
		qos = 0
	}

	return mqtt.PublishMessage(topic, message, qos)
}