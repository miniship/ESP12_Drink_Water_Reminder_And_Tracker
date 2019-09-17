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
	device := ctx.Param("device")
	readingList, err := interactor.ListAllWeightReading(device)

	if err == nil {
		ctx.JSON(http.StatusOK, readingList)
	} else {
		handleError(ctx, err)
	}
}

func CommandUserDevice(ctx *gin.Context) {
	device := ctx.Param("device")
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