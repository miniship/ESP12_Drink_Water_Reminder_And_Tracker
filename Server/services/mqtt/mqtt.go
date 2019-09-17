package mqtt

import (
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

const ConnectionString = "tcp://localhost:1883"
//const ConnectionString = "tcp://35.197.155.112:4443"
const DeviceTopic = "device/"

var mqttClient mqtt.Client

func init() {
	opts := mqtt.NewClientOptions()
	opts.SetCleanSession(false)
	opts.AddBroker(ConnectionString)
	opts.SetClientID("dwr-web-server")
	opts.SetConnectionLostHandler(func(client mqtt.Client, err error) {
		log.Warn(fmt.Sprintf("[mmqt init] Connection lost : %s", err.Error()))
	})

	mqttClient = mqtt.NewClient(opts)
	token := mqttClient.Connect()
	if token.Wait() && token.Error() != nil {
		log.Fatalf("[mmqt init] Fail to connect broker, %v", token.Error())
	}
}


func SubscribeForAllDevices() {
	allUser, err := mongodb.FindUserList(bson.D{})
	if err != nil {
		log.Fatal("[SubscribeForAllDevices]", err)
	}

	for _, user := range allUser {
		for _, device := range user.DeviceList {
			if err := SubscribeForDevice(device); err != nil {
				log.Fatal("[SubscribeForAllDevices]", err)
			}
		}
	}
}

func SubscribeForDevice(device string) error {
	token := mqttClient.Subscribe(DeviceTopic + device, byte(0), onIncomingDataReceived)
	if token.Wait() && token.Error() != nil {
		return token.Error()
	}
	return nil
}

func onIncomingDataReceived(client mqtt.Client, message mqtt.Message) {
	topic := message.Topic()
	if !strings.Contains(topic, DeviceTopic) || len(strings.Split(topic, "/")) != 2 {
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
	token := mqttClient.Unsubscribe(DeviceTopic + device)
	if token.Wait() && token.Error() != nil {
		return token.Error()
	}
	return nil
}

func PublishMessage(topic string, message string, qos int) error {
	token := mqttClient.Publish(topic, byte(qos), false, message)
	if token.Wait() && token.Error() != nil {
		return token.Error()
	}
	return nil
}