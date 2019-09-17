package validation

import (
	"Server/models"
	"github.com/go-playground/universal-translator"
	"github.com/labstack/gommon/log"
	"gopkg.in/go-playground/validator.v9"
	enTranslations "gopkg.in/go-playground/validator.v9/translations/en"
)

func ValidateCommand(command models.Command, translator ut.Translator) error {
	commandValidator := validator.New()

	if err := enTranslations.RegisterDefaultTranslations(commandValidator, translator); err != nil {
		log.Error("[ValidateCommand] error registering default translation", err)
	}

	if err := commandValidator.Struct(command); err != nil {
		return err
	}

	return nil
}