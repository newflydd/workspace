package models

import "encoding/xml"

type WxEvent struct {
	XMLName      xml.Name      `xml:"xml"`
	ToUserName   string        `xml:"ToUserName"`
	FromUserName string        `xml:"FromUserName"`
	CreateTime   int64         `xml:"CreateTime"`
	MsgType      string        `xml:"MsgType"`
	Event        string        `xml:"Event"` //VIEW
	EventKey     string        `xml:"EventKey"`
	MenuId       string        `xml:"MenuId"`
	ScanCodeInfo *ScanCodeInfo `xml:"ScanCodeInfo"` //专属于扫码
	Content      string        `xml:"Content"`
	Ticket       string        `xml:"Ticket"`
}

type ScanCodeInfo struct {
	ScanType   string `xml:"ScanType"`
	ScanResult string `xml:"ScanResult"`
}
