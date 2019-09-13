package models

type User struct {
	Username	string		`form:"username" json:"username" xml:"username" validate:"required,min=3,max=50"`
	Password	string		`form:"password" json:"password" xml:"password" validate:"required,min=8,max=50"`
	DeviceList	[]string	`form:"deviceList" json:"deviceList" xml:"deviceList"`
}