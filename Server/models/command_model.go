package models

type CommandCode int

const (
	RestartDevice		CommandCode = 1
	SwitchDeviceMode	CommandCode = 2
	SwitchAlertMode 	CommandCode = 3
)

func (code CommandCode) ToString() string {
	names := []string{
		"UnuseCode",
		"RestartDevice",
		"SwitchDeviceMode",
		"SwitchAlertMode",
	}

	return names[code]
}

type Command struct {
	Code		CommandCode	`form:"code" json:"code" xml:"code" validate:"required,numeric,min=1,max=3"`
	Parameters	[]string	`form:"parameters" json:"parameters" xml:"parameters"`
}
