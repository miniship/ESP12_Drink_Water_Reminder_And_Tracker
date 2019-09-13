package controllers

import (
	"Server/customerrors"
	"Server/models"
	"Server/services/interactor"
	"github.com/gin-gonic/gin"
	"net/http"
)

func Login(ctx *gin.Context)  {
	var user models.User

	if err := ctx.ShouldBindJSON(&user); err != nil {
		handleError(ctx, customerrors.Wrap(err, "error parsing json user"))
		return
	}

	jwt, err := interactor.UserLogin(user)
	if err != nil {
		handleError(ctx, err)
		return
	}

	ctx.JSON(http.StatusOK, jwt)
}

func Logout(ctx *gin.Context)  {
	ctx.JSON(http.StatusOK, "Logout not implement yet")
}
