package interactors

import (
	"Server/customerrors"
	"Server/models"
	"Server/services/mongodb"
	"Server/services/mqtt"
	"github.com/labstack/gommon/log"
	"go.mongodb.org/mongo-driver/bson"
	"strconv"
)

const commandTopic = "/3ml/command/"

func ListAllWeightReading(username string, device string) ([]models.Reading, error) {
	if !isDeviceBelongToUser(username, device) {
		return nil, customerrors.BadRequest.Newf("error list all weight reading of device %s for user %s, may be the device is not belong to the user", device, username)
	}

	filter := bson.M{"device":device}
	return mongodb.FindWeightReadingList(filter)
}

func ClearAllWeightReading(username string, device string) (int64, error) {
	if !isDeviceBelongToUser(username, device) {
		return 0, customerrors.BadRequest.Newf("error clear all weight reading of device %s for user %s, may be the device is not belong to the user", device, username)
	}

	filter := bson.M{"device":device}
	return mongodb.DeleteWeightReadingList(filter)
}

func CommandUserDevice(username string, device string, command models.Command) error {
	if !isDeviceBelongToUser(username, device) {
		return customerrors.BadRequest.Newf("error send command to device %s for user %s, may be the device is not belong to the user", device, username)
	}

	topic := commandTopic + device
	code := command.Code
	message := strconv.Itoa(int(code))
	message += ":"
	for _, param := range command.Parameters {
		message += param + ";"
	}

	log.Infof("Topic: %s, Mesage: %s", topic, message)
	return mqtt.PublishMessage(topic, message, 1)
}

func isDeviceBelongToUser(username string, device string) bool {
	if user, err := mongodb.FindUser(bson.M{"deviceList":device}); err == nil && user.Username == username {
		return true
	}

	return false
}