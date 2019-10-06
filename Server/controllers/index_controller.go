package controllers

import (
	"github.com/gin-gonic/gin"
	"net/http"
)

func Index(ctx *gin.Context) {
	ctx.JSON(http.StatusOK, gin.H{"message":"Welcome"})
}