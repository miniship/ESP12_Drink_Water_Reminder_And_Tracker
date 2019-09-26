package models

type CommandCode int

const (
	RestartDevice				CommandCode = 1
	UpdateSchedule				CommandCode = 2
	UpdateOfflineRemindPeriod 	CommandCode = 3
	Tare						CommandCode = 4
)

func (code CommandCode) ToString() string {
	names := []string{
		"UnuseCode",
		"RestartDevice",
		"UpdateSchedule",
		"UpdateOfflineRemindPeriod",
		"Tare",
	}

	return names[code]
}

type Command struct {
	Code		CommandCode	`form:"code" json:"code" xml:"code" validate:"required,numeric,min=1,max=4"`
	Parameters	[]string	`form:"parameters" json:"parameters" xml:"parameters"`
}
