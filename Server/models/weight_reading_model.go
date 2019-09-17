package models

import (
	"time"
)

type Reading struct {
	Device string `form:"device" json:"device" xml:"device"`
	WeightInGram int `form:"weight" json:"weight" xml:"weight"`
	Time time.Time `form:"time" json:"time" xml:"time"`
}
