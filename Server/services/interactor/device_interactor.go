package interactor

import (
	"Server/models"
	"Server/services/mongodb"
	"Server/services/mqtt"
	"go.mongodb.org/mongo-driver/bson"
	"strconv"
)

const CommandTopic = "command/"

func ListAllWeightReading(device string) ([]models.Reading, error) {
	filter := bson.M{"device":device}
	return mongodb.FindWeightReadingList(filter)
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
	case models.RestartDevice:
		qos = 0
		break
	case models.UpdateSchedule:
		qos = 2
		break
	default:
		qos = 0
	}

	return mqtt.PublishMessage(topic, message, qos)
}