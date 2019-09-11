package controller

import "net/http"

func Register(w http.ResponseWriter, r http.Request)  {
	w.Header().Set("Content-Type", "application/json")

}

func Login(w http.ResponseWriter, r http.Request)  {

}

func Logout(w http.ResponseWriter, r http.Request)  {

}
