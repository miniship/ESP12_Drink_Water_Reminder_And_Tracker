package controllers

import (
	"Server/customerrors"
	"github.com/gin-gonic/gin"
	"github.com/labstack/gommon/log"
	"net/http"
)

func handleError(ctx *gin.Context, err error) {
	var status int
	errorType := customerrors.GetType(err)

	switch errorType {
	case customerrors.BadRequest:
		status = http.StatusBadRequest
	case customerrors.NotFound:
		status = http.StatusNotFound
	default:
		status = http.StatusInternalServerError
	}

	log.Error("[handleError]", err.Error())
	ctx.JSON(status, err.Error())
}