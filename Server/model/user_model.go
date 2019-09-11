package model

type User struct {
	Username	string		`json:"username"`
	Password	string		`json:"password"`
	DeviceList	[]string	`json:"device_list"`
}