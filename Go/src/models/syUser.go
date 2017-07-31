package models

/**
 * 与赛洋MySQL数据库中的sy_user表对应
 */
type SyUser struct {
	Id          int32  `json:"id"`
	Name        string `json:"name"`
	Phone       string `json:"phone"`
	IdCardL4    string `json:"id_card_l4"`
	Dev         int8   `json:"dev"`
	Type        int8   `json:"type"`
	OpenId      string `json:"openid"`
	WxName      string `json:"wx_name"`
	RegDatetime int64  `json:"reg_datetime"`
}
