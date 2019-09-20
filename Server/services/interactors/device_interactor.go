package interactors

import (
	"Server/customerrors"
	"Server/models"
	"Server/services/mongodb"
	"Server/services/mqtt"
	"go.mongodb.org/mongo-driver/bson"
	"strconv"
)

const commandTopic = "command/"

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

func isDeviceBelongToUser(username string, device string) bool {
	if user, err := mongodb.FindUser(bson.M{"deviceList":device}); err == nil && user.Username == username {
		return true
	}

	return false
}