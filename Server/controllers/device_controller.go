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

func ListAllWeightReading(ctx *gin.Context) {
	username := ctx.Param("username")
	device := ctx.Param("device")

	readingList, err := interactors.ListAllWeightReading(username, device)
	if err == nil {
		ctx.JSON(http.StatusOK, readingList)
	} else {
		handleError(ctx, err)
	}
}

func ClearAllWeightReading(ctx *gin.Context) {
	username := ctx.Param("username")
	device := ctx.Param("device")

	count, err := interactors.ClearAllWeightReading(username, device)
	if err != nil {
		handleError(ctx, err)
	} else {
		ctx.JSON(http.StatusOK, fmt.Sprintf("delete %d weight reading successfully", count))
	}
}

func CommandUserDevice(ctx *gin.Context) {
	username := ctx.Param("username")
	device := ctx.Param("device")
	var command models.Command

	if err := ctx.ShouldBindJSON(&command); err != nil {
		handleError(ctx, customerrors.Wrap(err, "error parsing json command"))
		return
	}

	if err := customValidators.ValidateCommand(command); err != nil {
		log.Error("[CommandUserDevice]", err.Error())
		ctx.JSON(http.StatusBadRequest, err.(validator.ValidationErrors).Translate(customValidators.UniversalEnglishTranslator))
		return
	}

	if err := interactors.CommandUserDevice(username, device, command); err != nil {
		handleError(ctx, err)
		return
	}

	ctx.JSON(http.StatusOK, fmt.Sprintf("send command %s to device %s successfully", command.Code.ToString(), device))
}