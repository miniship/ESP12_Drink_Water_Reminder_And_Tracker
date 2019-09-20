package mqtt

import (
	"Server/customerrors"
	"Server/models"
	"Server/services/mongodb"
	"fmt"
	"github.com/eclipse/paho.mqtt.golang"
	"github.com/labstack/gommon/log"
	"go.mongodb.org/mongo-driver/bson"
	"strconv"
	"strings"
	"time"
)

const connectionString = "tcp://localhost:1883"
//const connectionString = "tcp://35.197.155.112:4443"
const deviceTopic = "device/"

var mqttClient mqtt.Client

func init() {
	opts := mqtt.NewClientOptions()
	opts.SetCleanSession(false)
	opts.AddBroker(connectionString)
	opts.SetClientID("dwr-web-server")
	opts.SetConnectionLostHandler(func(client mqtt.Client, err error) {
		log.Warn(fmt.Sprintf("[mqtt init] Connection lost : %s", err.Error()))
	})
	// the auto reconnect feature is default on will try and keep the connection going but cause the subscriptions to stop working
	opts.SetOnConnectHandler(SubscribeForAllDevices)

	mqttClient = mqtt.NewClient(opts)

	token := mqttClient.Connect()
	if token.Wait() && token.Error() != nil {
		log.Errorf("[mqtt init] Fail to connect broker, %v", token.Error())
	}
}

func SubscribeForAllDevices(client mqtt.Client) {
	allUser, err := mongodb.FindUserList(bson.D{})
	if err != nil {
		log.Error("[SubscribeForAllDevices]", err)
	}

	for _, user := range allUser {
		for _, device := range user.DeviceList {
			if err := SubscribeForDevice(device); err != nil {
				log.Error("[SubscribeForAllDevices]", err)
			}
		}
	}
}

func SubscribeForDevice(device string) error {
	token := mqttClient.Subscribe(deviceTopic + device, byte(0), onIncomingDataReceived)
	if token.Wait() && token.Error() != nil {
		return customerrors.Wrapf(token.Error(), "error subscribing topic for device %s", device)
	}

	return nil
}

func onIncomingDataReceived(client mqtt.Client, message mqtt.Message) {
	topic := message.Topic()
	if !strings.Contains(topic, deviceTopic) || len(strings.Split(topic, "/")) != 2 {
		log.Warn("[onIncomingDataReceived] receive message on invalid topic", topic)
		return
	}

	device := strings.Split(topic, "/")[1]
	payload := string(message.Payload())

	weightInGram, err := strconv.Atoi(payload)
	if err != nil {
		log.Warnf("[onIncomingDataReceived] receive invalid payload %s on topic %s", payload, topic)
		return
	}

	mongodb.InsertWeightReading(models.Reading{
		Device:       device,
		WeightInGram: weightInGram,
		Time:         time.Now(),
	})
}

func UnsubscribeForDevice(device string) error {
	token := mqttClient.Unsubscribe(deviceTopic + device)
	if token.WaitTimeout(10 * time.Second) && token.Error() != nil {
		return customerrors.Wrapf(token.Error(), "error un-subscribing topic for device %s", device)
	}

	return nil
}

func PublishMessage(topic string, message string, qos int) error {
	token := mqttClient.Publish(topic, byte(qos), false, message)
	if token.Wait() && token.Error() != nil {
		return customerrors.Wrapf(token.Error(), "error publishing message % to topic %s with quality %d", message, topic, qos)
	}

	return nil
}