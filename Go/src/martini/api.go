package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	//"io/ioutil"
	"github.com/martini-contrib/render"
	"models"
	"net/http"
)

func ApiGetStoreData(req *http.Request, mysqlConn *sql.DB) string {
	/* 检查参数 */
	req.ParseForm()

	if (req.Form["fnumber"] == nil || req.Form["fnumber"][0] == "") && (req.Form["fname"] == nil || req.Form["fname"][0] == "") && (req.Form["fstockname"] == nil || req.Form["fstockname"][0] == "") {
		fmt.Println("error, all paramters are nil")
		return "所有的参数都为空，无法查询。"
	}

	/* 根据接收的参数构造SQL */
	var fNumberSql, fNameSql, fStockNameSql string

	if req.Form["fnumber"] == nil || req.Form["fnumber"][0] == "" {
		fNumberSql = ""
	} else {
		fNumberSql = " FNumber = '" + req.Form["fnumber"][0] + "'"
	}

	if req.Form["fname"] == nil || req.Form["fname"][0] == "" {
		fNameSql = ""
	} else {
		if req.Form["fnumber"] == nil || req.Form["fnumber"][0] == "" {
			fNameSql = " FName like '%" + req.Form["fname"][0] + "%'"
		} else {
			fNameSql = " and FName like '%" + req.Form["fname"][0] + "%'"
		}
	}

	if req.Form["fstockname"] == nil || req.Form["fstockname"][0] == "" {
		fStockNameSql = ""
	} else {
		if (req.Form["fnumber"] == nil || req.Form["fnumber"][0] == "") && (req.Form["fname"] == nil || req.Form["fname"][0] == "") {
			fStockNameSql = " FStockName like '%" + req.Form["fstockname"][0] + "%'"
		} else {
			fStockNameSql = " and FStockName like '%" + req.Form["fstockname"][0] + "%'"
		}
	}

	//查询数据量
	fsqlCount := "select count(*) from sy_item where FQty <> 0 and" + fNumberSql + fNameSql + fStockNameSql

	rows, err := mysqlConn.Query(fsqlCount)
	checkErr(err)

	var count int
	rows.Next()
	err = rows.Scan(&count)

	if count > 100 {
		return "数据量太大，无法显示，请进一步约束条件再查询。"
	}

	fsql := "select * from sy_item where FQty <> 0 and" + fNumberSql + fNameSql + fStockNameSql

	fmt.Println(fsql)

	//查询数据
	rows, err = mysqlConn.Query(fsql)
	checkErr(err)

	fitems := []models.K3Item{} /* 动态数组，切片的使用，也可以使用container包中的list */

	for rows.Next() {
		fitem := models.K3Item{}
		err = rows.Scan(&fitem.FItemID, &fitem.FNumber, &fitem.FUnitName, &fitem.FName, &fitem.FModel, &fitem.FQty, &fitem.FStockName)
		checkErr(err)
		fitems = append(fitems, fitem) /* 切片的追加，使用独立函数append，切片的长度可用len()求出 */
	}

	b, err := json.Marshal(fitems)
	checkErr(err)

	rows.Close()

	return string(b)
}

func ApiGetUpdateTime(mysqlConn *sql.DB) string {
	//查询数据
	rows, err := mysqlConn.Query("select s.`value` from sy_system s where s.`key`='update_time'")
	checkErr(err)

	rows.Next()
	var timeStr string
	err = rows.Scan(&timeStr)
	checkErr(err)

	rows.Close()

	return timeStr
}

func ApiRegister(req *http.Request, mysqlConn *sql.DB) string {
	phone, idCardL4, openid, nickname := req.PostFormValue("phone"), req.PostFormValue("id_card_l4"), req.PostFormValue("openid"), req.PostFormValue("nickname")
	if phone == "" || idCardL4 == "" || openid == "" || nickname == "" {
		return "出错，清检查输入的参数。"
	}

	/* 检查phone和l4是否在mysql数据库中 */
	stmt, err := mysqlConn.Prepare("select id,name from sy_user u where u.phone = ? and u.id_card_l4 = ?")
	checkErr(err)

	rows, err := stmt.Query(phone, idCardL4)
	checkErr(err)

	if rows.Next() {
		var syUser = models.SyUser{}
		err = rows.Scan(&syUser.Id, &syUser.Name)

		/* 跟新该记录的openid, wxname, regdatetime */
		stmt, err = mysqlConn.Prepare(`
			update sy_user set 
				openid = ?, 
				wx_name = ?, 
				reg_datetime = UNIX_TIMESTAMP() 
			where id = ?`)
		checkErr(err)

		_, err = stmt.Exec(openid, nickname, syUser.Id)
		checkErr(err)

		return `{"name":"` + syUser.Name + `"}`
	}

	rows.Close()
	stmt.Close()
	return "未找到匹配数据，请与管理部联系添加员工信息。"
}

func checkHasBindToError(wxUserInfo *models.WxUserInfo, mysqlConn *sql.DB, r render.Render) {
	stmt, err := mysqlConn.Prepare("select id from sy_user u where u.openid = ?")
	checkErr(err)
	defer stmt.Close()

	rows, err := stmt.Query(wxUserInfo.OpenId)
	checkErr(err)
	defer rows.Close()

	if rows.Next() {
		r.HTML(200, "error", "您的账号已绑定，如需解绑，请联系管理部。")
	}
}

func checkHasBind(wxUserInfo *models.WxUserInfo, mysqlConn *sql.DB, r render.Render) {
	stmt, err := mysqlConn.Prepare("select name from sy_user u where u.openid = ?")
	checkErr(err)
	defer stmt.Close()

	rows, err := stmt.Query(wxUserInfo.OpenId)
	checkErr(err)
	defer rows.Close()

	if !rows.Next() {
		r.HTML(200, "error", "您的账号尚未绑定，请返回公众号进行账号绑定。")
	}

	var name string
	rows.Scan(&name)
	wxUserInfo.Name = name
}
