package main

import (
	"Server/controllers"
	"Server/services/jwtService"
	"github.com/appleboy/gin-jwt/v2"
	"github.com/gin-gonic/gin"
	"github.com/labstack/gommon/log"
	"net/http"
)

func main() {
	//mqtt.SubscribeForAllDevices()
	router := gin.Default()
	jwtMiddleware := jwtService.GetJWTMiddleware()

	router.NoRoute(jwtMiddleware.MiddlewareFunc(), noRouteHandler)
	router.GET("/", controllers.Index)
	router.POST("/register", controllers.RegisterUser)
	router.POST("/login", jwtMiddleware.LoginHandler)

	authRouter := router.Group("/auth")
	authRouter.Use(jwtMiddleware.MiddlewareFunc())
	authRouter.GET("/listUsers", controllers.ListAllUsers)

	userApi := authRouter.Group("/user")
	userApi.POST("/:username/delete", controllers.DeleteUser)
	userApi.GET("/:username/listDevices", controllers.ListUserDevices)

	deviceApi := userApi.Group("/:username/device")
	deviceApi.GET("/:device/listReadings", controllers.ListAllWeightReading)
	deviceApi.POST("/:device/clearReadings", controllers.ClearAllWeightReading) // for test
	deviceApi.POST("/:device/add", controllers.AddUserDevice)
	deviceApi.POST("/:device/remove", controllers.RemoveUserDevice)
	deviceApi.POST("/:device/command", controllers.CommandUserDevice)

	router.Run(":8080")
}

func noRouteHandler(c *gin.Context) {
	claims := jwt.ExtractClaims(c)
	log.Info("[noRouteHandler] NoRoute claims: %#v\n", claims)
	c.JSON(404, gin.H{
		"code": http.StatusNotFound,
		"message": "Page not found",
	})
}