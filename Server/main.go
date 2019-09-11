package main

import (
	"Server/collection"
	"go.mongodb.org/mongo-driver/bson"
	"log"
)

func main()  {
	//	router := mux.NewRouter()
	//	router.HandleFunc("/register", controller.Register).Methods("POST")
	//	router.HandleFunc("/login", controller.Login).Methods("POST")
	//	router.HandleFunc("/logout", controller.Logout).Methods("POST")
	//
	//	log.Fatal(http.ListenAndServe(":8080", router))

	filter := bson.M{"username":"Thanh"}
	// add an array to an array
	//update := bson.M{"$addToSet":bson.M{"devicelist":bson.M{"$each":bson.A{"XXX", "YYY"}}}}

	// remove a field
	//update := bson.M{"$unset":bson.M{"device_list":""}}

	//set new value for a field
	//update := bson.M{"$set":bson.M{"devicelist":bson.A{"BBB"}}}

	//matched, updated := collection.UpsertUser(filter, update)
	//log.Println("Match: ", matched)
	//log.Println("Updated: ", updated)

	isSuccess := collection.DeleteUser(filter)
	if isSuccess {
		log.Println("success delete user")
	} else {
		log.Println("failed delete user")
	}
}
