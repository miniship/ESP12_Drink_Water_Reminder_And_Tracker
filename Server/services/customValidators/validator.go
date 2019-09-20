package customValidators

import (
	"Server/models"
	"github.com/go-playground/locales/en"
	"github.com/go-playground/universal-translator"
	"github.com/labstack/gommon/log"
	"gopkg.in/go-playground/validator.v9"
	enTranslations "gopkg.in/go-playground/validator.v9/translations/en"
)

var UniversalEnglishTranslator ut.Translator
var structValidate *validator.Validate

func init() {
	enLocaleTranslator := en.New()
	universalTranslator := ut.New(enLocaleTranslator, enLocaleTranslator)

	enTranslator, found := universalTranslator.GetTranslator("en")
	if !found {
		println("[validators init] universal english translator not found")
	}
	UniversalEnglishTranslator = enTranslator

	structValidate = validator.New()
	if err := enTranslations.RegisterDefaultTranslations(structValidate, UniversalEnglishTranslator); err != nil {
		log.Error("[validators init] error registering default translation", err)
	}
}

func ValidateUser(user models.User) error {
	if err := structValidate.Struct(user); err != nil {
		return err
	}
	return nil
}

func ValidateCommand(command models.Command) error {
	if err := structValidate.Struct(command); err != nil {
		return err
	}
	return nil
}
