package controllers

import (
	"fmt"
	"github.com/gin-gonic/gin"
	"net/http"
	"time"
)

func Index(ctx *gin.Context) {
	ctx.JSON(http.StatusOK, gin.H{"message":"Welcome"})
}

func GetCurrentTime(ctx *gin.Context)  {
	hours, minutes, _ := time.Now().Clock();
	ctx.String(http.StatusOK, fmt.Sprintf("%d:%d", hours, minutes))
}
