package main

import (
	"encoding/json"
	"fmt"
	"github.com/jinzhu/gorm"
	"github.com/martini-contrib/render"
	"net/http"
)

func ApiGetStoreData(req *http.Request, mysqlConn *gorm.DB) string {
	/* 检查参数 */
	req.ParseForm()

	if (req.Form["fnumber"] == nil || req.Form["fnumber"][0] == "") && (req.Form["fname"] == nil || req.Form["fname"][0] == "") && (req.Form["fstockname"] == nil || req.Form["fstockname"][0] == "") {
		fmt.Println("error, all paramters are nil")
		return "所有的参数都为空，无法查询。"
	}

	theConn := mysqlConn

	if req.Form["fnumber"] != nil && req.Form["fnumber"][0] != "" {
		theConn = theConn.Where("FNumber = ?", req.Form["fnumber"][0])
	}

	if req.Form["fname"] != nil && req.Form["fname"][0] != "" {
		theConn = theConn.Where("FName like ?", "%"+req.Form["fname"][0]+"%")
	}

	if req.Form["fstockname"] != nil && req.Form["fstockname"][0] != "" {
		theConn = theConn.Where("FStockName like ?", "%"+req.Form["fstockname"][0]+"%")
	}

	/* 查询个数 */
	var count int
	theConn.Model(&SyItem{}).Count(&count)

	if count > 100 {
		return "数据量太大，无法显示，请进一步约束条件再查询。"
	}

	/* 查询数据 */
	fitems := []SyItem{} /* 动态数组，切片的使用，也可以使用container包中的list */
	theConn.Find(&fitems)

	jsonstr, err := json.Marshal(fitems)
	checkErr(err)

	return string(jsonstr)
}

/* 查询更新时间 */
func ApiGetUpdateTime(mysqlConn *gorm.DB) string {
	var timeKV SySystem
	mysqlConn.Select("value").Where("`key` = ?", "update_time").First(&timeKV)

	return timeKV.Value
}

/* 用户账号绑定 */
func ApiRegister(req *http.Request, mysqlConn *gorm.DB) string {
	phone, idCardL4, openid, nickname := req.PostFormValue("phone"), req.PostFormValue("id_card_l4"), req.PostFormValue("openid"), req.PostFormValue("nickname")
	if phone == "" || idCardL4 == "" || openid == "" || nickname == "" {
		return "出错，清检查输入的参数。"
	}

	/* 检查phone和l4是否在mysql数据库中 */
	syUser := SyUser{}
	mysqlConn.Select("id,name").Where("phone = ?", phone).Where("id_card_l4 = ?", idCardL4).First(&syUser)

	if syUser.Name != "" {
		/* 跟新该记录的openid, wxname, regdatetime */
		mysqlConn.Model(&syUser).Updates(
			map[string]interface{}{
				"openid":       openid,
				"wx_name":      nickname,
				"reg_datetime": gorm.Expr("UNIX_TIMESTAMP()")})

		return `{"name":"` + syUser.Name + `"}`
	}

	return "未找到匹配数据，请与管理部联系添加员工信息。"
}

/* 检索是否绑定，如果账号已绑定就路由到error页面 */
func checkHasBindToError(syUser *SyUser, mysqlConn *gorm.DB, r render.Render) {
	mysqlConn.Select("id").Where("openid = ?", syUser.Openid).First(syUser)

	if syUser.Id != 0 {
		r.HTML(200, "error", "您的账号已绑定，如需解绑，请联系管理部。")
	}
}

func checkHasBind(syUser *SyUser, mysqlConn *gorm.DB, r render.Render) {
	mysqlConn.Select("name").Where("openid = ?", syUser.Openid).First(syUser)

	if syUser.Name == "" {
		r.HTML(200, "error", "您的账号尚未绑定，请返回公众号进行账号绑定。")
	}
}
