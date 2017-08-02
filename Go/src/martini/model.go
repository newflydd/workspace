package main

import "encoding/xml"

/* sy_system表 */
type SySystem struct {
	Id    int64
	Key   string
	Value string
}

/* sy_user表，同时可以映射微信返回的用户信息结构 */
type SyUser struct {
	Id          int64  `json:"id"`
	Name        string `json:"name"`
	Phone       string `json:"phone"`
	IdCardL4    string `json:"id_card_l4"`
	Dev         int8   `json:"dev"`
	Type        int8   `json:"type"`
	Openid      string `json:"openid"`
	WxName      string `json:"wx_name"`
	RegDatetime int64  `json:"reg_datetime"`

	NickName   string   `gorm:"-" json:"nickname"`
	Sex        int      `gorm:"-" json:"sex"`
	Province   string   `gorm:"-" json:"province"`
	City       string   `gorm:"-" json:"city"`
	Country    string   `gorm:"-" json:"country"`
	HeadImgUrl string   `gorm:"-" json:"headimgurl"`
	Privilege  []string `gorm:"-" json:"privilege"`
	Unionid    string   `gorm:"-" json:"unionid"`
	Code       string   `gorm:"-"`
}

/* sy_item表 */
type SyItem struct {
	FItemID    int     `json:"fitemid"    gorm:"column:FItemId"`
	FNumber    string  `json:"fnumber"    gorm:"column:FNumber"`
	FUnitName  string  `json:"funitname"  gorm:"column:FUnitName"`
	FName      string  `json:"fname"      gorm:"column:FName"`
	FModel     string  `json:"fmodel"     gorm:"column:FModel"`
	FQty       float32 `json:"fqty"       gorm:"column:FQty"`
	FStockName string  `json:"fstockname" gorm:"column:FStockName"`
}

/* 微信返回的事件结构 */
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

/* 上面的结构要用，扫码相关 */
type ScanCodeInfo struct {
	ScanType   string `xml:"ScanType"`
	ScanResult string `xml:"ScanResult"`
}

/* 微信进入WEB页面时换取的access_token and open_id */
type WxAtoiResp struct {
	AccessToken  string `json:"access_token"`
	ExpiresIn    int    `json:"expires_in"`
	RefreshToken string `json:"refresh_token"`
	OpenId       string `json:"openid"`
	Scope        string `json:"scope"`
}

/* 请求微信总服务AccessToken时返回的数据结构 */
type WxAccessTokenResp struct {
	AccessToken string `json:"access_token"`
	ExpiresIn   int    `json:"expires_in"`
}

/* 请求微信JSTicket时返回的数据结构 */
type WxJSTicketJsonResp struct {
	ErrCode   int    `json:"errcode"`
	ErrMsg    string `json:"errmsg"`
	Ticket    string `json:"ticket"`
	ExpiresIn int    `json:"expires_in"`
}
