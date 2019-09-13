package jwtService

import "Server/models"

func GenerateJwt(user models.User) (string, error) {
	return "Dummy jwt", nil
}