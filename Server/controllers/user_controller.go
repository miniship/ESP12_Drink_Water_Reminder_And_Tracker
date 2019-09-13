package controllers

import (
	"Server/customerrors"
	"Server/models"
	"Server/services/interactor"
	"Server/services/validation"
	"fmt"
	"github.com/gin-gonic/gin"
	"github.com/go-playground/locales/en"
	"github.com/go-playground/universal-translator"
	"gopkg.in/go-playground/validator.v9"
	"log"
	"net/http"
)

func RegisterUser(ctx *gin.Context)  {
	var user models.User

	if err := ctx.ShouldBindJSON(&user); err != nil {
		handleError(ctx, customerrors.Wrap(err, "error parsing json user"))
		return
	}

	if !validateUserInput(ctx, user) {
		return
	}

	if err := interactor.RegisterUser(user); err != nil {
		handleError(ctx, err)
		return
	}

	ctx.JSON(http.StatusOK, fmt.Sprintf("register new user with name %s successfully", user.Username))
}

func ListAllUsers(ctx *gin.Context) {
	userList, err := interactor.ListAllUser()

	if err == nil {
		ctx.JSON(http.StatusOK, userList)
	} else {
		handleError(ctx, err)
	}
}

func ListUserDevices(ctx *gin.Context) {
	username := ctx.Param("username")
	deviceList, err := interactor.ListUserDevices(username)

	if err == nil {
		ctx.JSON(http.StatusOK, deviceList)
	} else {
		handleError(ctx, err)
	}
}

func UpdateUserDevices(ctx *gin.Context) {
	username := ctx.Param("username")
	var user models.User

	if err := ctx.ShouldBindJSON(&user); err != nil {
		handleError(ctx, customerrors.Wrap(err, "error parsing device list"))
		return
	}

	if err := interactor.UpdateUserDevices(username, user.DeviceList); err != nil {
		handleError(ctx, err)
		return
	}

	ctx.JSON(http.StatusOK, fmt.Sprintf("update device list for user with name %s successfully", username))
}

func DeleteUser(ctx *gin.Context) {
	username := ctx.Param("username")
	count, err := interactor.DeleteUserByName(username)
	if err != nil {
		handleError(ctx, err)
	} else {
		ctx.JSON(http.StatusOK, fmt.Sprintf("delete %d user with name %s successfully", count, username))
	}
}

func validateUserInput(ctx *gin.Context, user models.User) bool {
	enLocaleTranslator := en.New()
	universalTranslator := ut.New(enLocaleTranslator, enLocaleTranslator)
	translator, found := universalTranslator.GetTranslator("en")
	if !found {
		println("[RegisterUser] english translator not found")
	}

	if err := validation.ValidateUser(user, translator); err != nil {
		log.Println("[validateUserInput]", err.Error())
		ctx.JSON(http.StatusBadRequest, err.(validator.ValidationErrors).Translate(translator))
		return false
	}

	return true
}