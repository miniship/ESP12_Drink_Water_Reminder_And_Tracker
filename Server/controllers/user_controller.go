package controllers

import (
	"Server/customerrors"
	"Server/models"
	"Server/services/customValidators"
	"Server/services/interactors"
	"fmt"
	"github.com/gin-gonic/gin"
	"github.com/labstack/gommon/log"
	"gopkg.in/go-playground/validator.v9"
	"net/http"
)

func RegisterUser(ctx *gin.Context)  {
	var user models.User

	if err := ctx.ShouldBindJSON(&user); err != nil {
		handleError(ctx, customerrors.Wrap(err, "error parsing json user"))
		return
	}

	if err := customValidators.ValidateUser(user); err != nil {
		log.Error("[RegisterUser]", err.Error())
		ctx.JSON(http.StatusBadRequest, err.(validator.ValidationErrors).Translate(customValidators.UniversalEnglishTranslator))
		return
	}

	if err := interactors.RegisterUser(user); err != nil {
		handleError(ctx, err)
		return
	}

	ctx.JSON(http.StatusOK, fmt.Sprintf("register new user %s successfully", user.Username))
}

func ListAllUsers(ctx *gin.Context) {
	userList, err := interactors.ListAllUser()
	if err == nil {
		ctx.JSON(http.StatusOK, userList)
	} else {
		handleError(ctx, err)
	}
}

func DeleteUser(ctx *gin.Context) {
	username := ctx.Param("username")
	if err := interactors.DeleteUserByName(username); err != nil {
		handleError(ctx, err)
	} else {
		ctx.JSON(http.StatusOK, fmt.Sprintf("delete user with name %s successfully", username))
	}
}

func ListUserDevices(ctx *gin.Context) {
	username := ctx.Param("username")

	deviceList, err := interactors.ListUserDevices(username)
	if err == nil {
		ctx.JSON(http.StatusOK, deviceList)
	} else {
		handleError(ctx, err)
	}
}

func AddUserDevice(ctx *gin.Context) {
	username := ctx.Param("username")
	device := ctx.Param("device")

	if err := interactors.AddUserDevice(username, device); err == nil {
		ctx.JSON(http.StatusOK, fmt.Sprintf("add device %s for user %s successfully", device, username))
	} else {
		handleError(ctx, err)
	}
}

func RemoveUserDevice(ctx *gin.Context) {
	username := ctx.Param("username")
	device := ctx.Param("device")

	if err := interactors.RemoveUserDevice(username, device); err == nil {
		ctx.JSON(http.StatusOK, fmt.Sprintf("remove device %s for user %s successfully", device, username))
	} else {
		handleError(ctx, err)
	}
}