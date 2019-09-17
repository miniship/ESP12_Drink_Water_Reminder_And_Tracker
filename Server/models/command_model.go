package models

type CommandCode int

const (
	RestartDevice	CommandCode = 1
	UpdateSchedule  CommandCode = 2
)

func (code CommandCode) ToString() string {
	names := []string{
		"UnuseCode",
		"RestartDevice",
		"UpdateSchedule",
	}

	return names[code]
}

type Command struct {
	Code		CommandCode	`form:"code" json:"code" xml:"code" validate:"required"`	// command code has 1 digit [1-9]
	Parameters	[]string	`form:"parameters" json:"parameters" xml:"parameters"`
}
