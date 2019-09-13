package main

import (
	"Server/controllers"
	"github.com/gin-gonic/gin"
	"net/http"
)

type RequestData struct {
	Method  string              `json:"method"`
	Headers http.Header         `json:"headers"`
	Body    string              `json:"body"`
	Path    string              `json:"path"`
	Queries map[string][]string `json:"queries"`
}

type ResponseData struct {
	Status  string      `json:"status"`
	Headers http.Header `json:"headers"`
	Body    string      `json:"body"`
}

func main() {
	router := gin.Default()
	api := router.Group("/api")
	api.GET("/", controllers.Index)
	api.POST("/login", controllers.Login)
	api.POST("/logout", controllers.Logout)
	api.POST("/register", controllers.RegisterUser)
	api.GET("/list", controllers.ListAllUsers)

	userApi := api.Group("/user")
	userApi.GET("/:username/devices/list", controllers.ListUserDevices)
	userApi.POST("/:username/devices/update", controllers.UpdateUserDevices)
	userApi.POST("/:username/delete", controllers.DeleteUser)

	router.Run(":3000")
}

func abc()  {
	//filter := bson.M{"username":"Thanh"}
	// add an array to an array
	//update := bson.M{"$addToSet":bson.M{"devicelist":bson.M{"$each":bson.A{"XXX", "YYY"}}}}

	// remove a field
	//update := bson.M{"$unset":bson.M{"device_list":""}}

	//set new value for a field
	//update := bson.M{"$set":bson.M{"devicelist":bson.A{"BBB"}}}

	//matched, updated := collection.UpsertUser(filter, update)
	//log.Println("Match: ", matched)
	//log.Println("Updated: ", updated)

	//isSuccess := collection.DeleteUser(filter)
	//if isSuccess {
	//	log.Println("success delete user")
	//} else {
	//	log.Println("failed delete user")
	//}
}
