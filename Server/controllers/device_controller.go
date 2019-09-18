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
	"github.com/labstack/gommon/log"
	"gopkg.in/go-playground/validator.v9"
	"net/http"
)

func ListAllWeightReading(ctx *gin.Context) {
	username := ctx.Param("username")
	device := ctx.Param("device")

	deviceList, err := interactor.ListUserDevices(username)
	if err != nil {
		handleError(ctx, err)
		return
	}

	for index, item := range deviceList {
		if device == item {
			break
		}
		if index == len(deviceList) - 1 {
			handleError(ctx, customerrors.NotFound.Newf("user %s does not have device %s", username, device))
			return
		}
	}

	readingList, err := interactor.ListAllWeightReading(device)
	if err == nil {
		ctx.JSON(http.StatusOK, readingList)
	} else {
		handleError(ctx, err)
	}
}

func ClearAllWeightReading(ctx *gin.Context) {
	username := ctx.Param("username")
	device := ctx.Param("device")

	deviceList, err := interactor.ListUserDevices(username)
	if err != nil {
		handleError(ctx, err)
		return
	}

	for index, item := range deviceList {
		if device == item {
			break
		}
		if index == len(deviceList) - 1 {
			handleError(ctx, customerrors.NotFound.Newf("user %s does not have device %s", username, device))
			return
		}
	}

	count, err := interactor.ClearAllWeightReading(device)
	if err != nil {
		handleError(ctx, err)
	} else {
		ctx.JSON(http.StatusOK, fmt.Sprintf("delete %d weight reading successfully", count))
	}
}

func CommandUserDevice(ctx *gin.Context) {
	username := ctx.Param("username")
	device := ctx.Param("device")

	deviceList, err := interactor.ListUserDevices(username)
	if err != nil {
		handleError(ctx, err)
		return
	}

	for index, item := range deviceList {
		if device == item {
			break
		}
		if index == len(deviceList) - 1 {
			handleError(ctx, customerrors.NotFound.Newf("user %s does not have device %s", username, device))
			return
		}
	}

	var command models.Command

	if err := ctx.ShouldBindJSON(&command); err != nil {
		handleError(ctx, customerrors.Wrap(err, "error parsing json command"))
		return
	}

	if !validateCommandInput(ctx, command) {
		return
	}

	if err := interactor.CommandUserDevice(device, command); err != nil {
		handleError(ctx, err)
		return
	}

	ctx.JSON(http.StatusOK, fmt.Sprintf("send command %s to device %s successfully", command.Code.ToString(), device))
}

func validateCommandInput(ctx *gin.Context, command models.Command) bool {
	enLocaleTranslator := en.New()
	universalTranslator := ut.New(enLocaleTranslator, enLocaleTranslator)

	translator, found := universalTranslator.GetTranslator("en")
	if !found {
		println("[validateCommandInput] english translator not found")
	}

	if err := validation.ValidateCommand(command, translator); err != nil {
		log.Error("[validateCommandInput]", err.Error())
		ctx.JSON(http.StatusBadRequest, err.(validator.ValidationErrors).Translate(translator))
		return false
	}

	return true
}