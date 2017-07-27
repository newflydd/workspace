package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	_ "github.com/go-sql-driver/mysql"
	"models"
	"net/http"
)

func ApiGetStoreData(req *http.Request) string {
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

	mysqlConn, err := sql.Open("mysql", "newflypig:jy.8215085@tcp(58.222.139.38:3306)/sywx?charset=utf8")
	checkErr(err)
	defer mysqlConn.Close()

	//查询数据量
	fsqlCount := "select count(*) from sy_item where" + fNumberSql + fNameSql + fStockNameSql

	rows, err := mysqlConn.Query(fsqlCount)
	checkErr(err)

	var count int
	rows.Next()
	err = rows.Scan(&count)

	if count > 100 {
		return "数据量太大，无法显示，请进一步约束条件再查询。"
	}

	fsql := "select * from sy_item where" + fNumberSql + fNameSql + fStockNameSql

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

	return string(b)
}

func ApiGetUpdateTime() string {
	mysqlConn, err := sql.Open("mysql", "newflypig:jy.8215085@tcp(58.222.139.38:3306)/sywx?charset=utf8")
	checkErr(err)
	defer mysqlConn.Close()

	//查询数据
	rows, err := mysqlConn.Query("select s.`value` from sy_system s where s.`key`='update_time'")
	checkErr(err)

	rows.Next()
	var timeStr string
	err = rows.Scan(&timeStr)
	checkErr(err)

	return timeStr
}
