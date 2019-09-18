package models

type CommandCode int

const (
	RestartDevice	CommandCode = 1
	UpdateSchedule  CommandCode = 2
	AlertUser 		CommandCode = 3
)

func (code CommandCode) ToString() string {
	names := []string{
		"RestartDevice",
		"UpdateSchedule",
		"AlertUser",
	}

	return names[code]
}

type Command struct {
	Code		CommandCode	`form:"code" json:"code" xml:"code" validate:"required,numeric,min=1,max=9"`
	Parameters	[]string	`form:"parameters" json:"parameters" xml:"parameters"`
}
