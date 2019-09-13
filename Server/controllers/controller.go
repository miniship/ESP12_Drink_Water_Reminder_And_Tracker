package controllers

import (
	"Server/customerrors"
	"github.com/gin-gonic/gin"
	"log"
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

	log.Println(err.Error())
	ctx.JSON(status, err.Error())
}