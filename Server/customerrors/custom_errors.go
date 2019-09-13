package customerrors

import (
	"fmt"
	"github.com/pkg/errors"
)

type ErrorType uint

const (
	NoType ErrorType = iota
	BadRequest
	NotFound
)

type customError struct {
	errorType ErrorType
	originalError error
}

// New creates a new customError
func (errorType ErrorType) New(msg string) error {
	return customError{errorType: errorType, originalError: errors.New(msg)}
}

// New creates a new customError with formatted message
func (errorType ErrorType) Newf(msg string, args ...interface{}) error {
	return customError{errorType: errorType, originalError: fmt.Errorf(msg, args...)}
}

// Wrap creates a new wrapped error
func (errorType ErrorType) Wrap(err error, msg string) error {
	return errorType.Wrapf(err, msg)
}

// Wrap creates a new wrapped error with formatted message
func (errorType ErrorType) Wrapf(err error, msg string, args ...interface{}) error {
	return customError{errorType: errorType, originalError: errors.Wrapf(err, msg, args...)}
}

// Error returns the message of a customError
func (error customError) Error() string {
	return error.originalError.Error()
}

// New creates a no type error
func New(msg string) error {
	return customError{errorType: NoType, originalError: errors.New(msg)}
}

// Newf creates a no type error with formatted message
func Newf(msg string, args ...interface{}) error {
	return customError{errorType: NoType, originalError: errors.New(fmt.Sprintf(msg, args...))}
}

// Wrap an error with a string
func Wrap(err error, msg string) error {
	return Wrapf(err, msg)
}

// Cause gives the original error
func Cause(err error) error {
	return errors.Cause(err)
}

// Wrapf an error with format string
func Wrapf(err error, msg string, args ...interface{}) error {
	wrappedError := errors.Wrapf(err, msg, args...)
	if customErr, ok := err.(customError); ok {
		return customError{
			errorType: customErr.errorType,
			originalError: wrappedError,
		}
	}

	return customError{errorType: NoType, originalError: wrappedError}
}

// GetType returns the error type
func GetType(err error) ErrorType {
	if customErr, ok := err.(customError); ok {
		return customErr.errorType
	}

	return NoType
}