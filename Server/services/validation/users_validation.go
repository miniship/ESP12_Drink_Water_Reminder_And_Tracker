package validation

import (
	"Server/models"
	"github.com/go-playground/universal-translator"
	"github.com/labstack/gommon/log"
	"gopkg.in/go-playground/validator.v9"
	enTranslations "gopkg.in/go-playground/validator.v9/translations/en"
)

func ValidateUser(user models.User, translator ut.Translator) error {
	userValidator := validator.New()

	if err := enTranslations.RegisterDefaultTranslations(userValidator, translator); err != nil {
		log.Error("[ValidateUser] error registering default translation", err)
	}

	if err := userValidator.Struct(user); err != nil {
		return err
	}

	return nil
}
