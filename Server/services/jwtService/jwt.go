package jwtService

import (
	"Server/models"
	"Server/services/mongodb"
	"github.com/appleboy/gin-jwt/v2"
	"github.com/gin-gonic/gin"
	"github.com/labstack/gommon/log"
	"go.mongodb.org/mongo-driver/bson"
	"golang.org/x/crypto/bcrypt"
	"time"
)

var identityKey = "id"

func GetJWTMiddleware() *jwt.GinJWTMiddleware {
	authMiddleware, err := jwt.New(&jwt.GinJWTMiddleware{
		Realm:       "gin jwt",
		Key:         []byte("server secret key use for signing"),
		Timeout:     time.Hour,
		Authenticator: authenticator,
		Authorizator: authorizator,
		PayloadFunc: payloadFunc,
		Unauthorized: unauthorized,
		IdentityHandler: identityHandler,
		IdentityKey: identityKey,
		TokenLookup: "header: Authorization",
		TokenHeadName: "Bearer",
		TimeFunc: time.Now,
	})

	if err != nil {
		log.Error("[GetJWTMiddleware]", err.Error())
	}

	return authMiddleware
}

func authenticator(ctx *gin.Context) (interface{}, error) {
	var user models.User
	if err := ctx.ShouldBind(&user); err != nil {
		return "", jwt.ErrMissingLoginValues
	}

	foundUser, err := mongodb.FindUser(bson.M{"username":user.Username})

	if err != nil || !comparePasswords(foundUser.Password, []byte(user.Password)) {
		return nil, jwt.ErrFailedAuthentication
	}

	return &models.User{
		Username:  user.Username,
	}, nil
}

func comparePasswords(hashedPwd string, plainPwd []byte) bool {
	byteHash := []byte(hashedPwd)
	err := bcrypt.CompareHashAndPassword(byteHash, plainPwd)
	if err != nil {
		log.Error("[comparePasswords]", err)
		return false
	}

	return true
}

func authorizator(data interface{}, ctx *gin.Context) bool {
	user, ok := data.(*models.User)
	username := ctx.Param("username")

	if ok && (user.Username == "admin" || user.Username == username) {
		return true
	}

	return false
}

func payloadFunc(data interface{}) jwt.MapClaims {
	if user, ok := data.(*models.User); ok {
		return jwt.MapClaims{
			identityKey: user.Username,
			"username": user.Username,
		}
	}

	return jwt.MapClaims{}
}

func unauthorized(ctx *gin.Context, code int, message string) {
	ctx.JSON(code, gin.H{
		"code":    code,
		"message": message,
	})
}

func identityHandler(ctx *gin.Context) interface{} {
	claims := jwt.ExtractClaims(ctx)
	return &models.User{
		Username: claims[identityKey].(string),
	}
}