package models

/**
 * 微信用户数据模型
 * 参考：https://mp.weixin.qq.com/wiki?t=resource/res_main&id=mp1421140842
 * 参数			描述
 * openid		用户的唯一标识
 * nickname		用户昵称
 * sex			用户的性别，值为1时是男性，值为2时是女性，值为0时是未知
 * province		用户个人资料填写的省份
 * city			普通用户个人资料填写的城市
 * country		国家，如中国为CN
 * headimgurl	用户头像，最后一个数值代表正方形头像大小（有0、46、64、96、132数值可选，0代表640*640正方形头像），用户没有头像时该项为空。若用户更换头像，原有头像URL将失效。
 * privilege	用户特权信息，json 数组，如微信沃卡用户为（chinaunicom）
 * unionid		只有在用户将公众号绑定到微信开放平台帐号后，才会出现该字段。
 */
type WxUserInfo struct {
	OpenId     string   `json:"openid"`
	NickName   string   `json:"nickname"`
	Sex        int      `json:"sex"`
	Province   string   `json:"province"`
	City       string   `json:"city"`
	Country    string   `json:"country"`
	HeadImgUrl string   `json:"headimgurl"`
	privilege  []string `json:"privilege"`
	unionid    string   `json:"unionid"`
}
