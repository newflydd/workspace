package main

import (
	"database/sql"
	"encoding/json"
	"fmt"
	_ "github.com/go-sql-driver/mysql"
	"models"
)

var (
	dbhostsip  = "127.0.0.1:3306" //IP地址
	dbusername = "root"           //用户名
	dbpassword = "123456"         //密码
	dbname     = "Test"           //表名
)

func main() {
	mysqlConn, err := sql.Open("mysql", "newflypig:jy.8215085@tcp(58.222.139.38:3306)/sywx?charset=utf8")
	checkErr(err)
	defer mysqlConn.Close()

	//查询数据
	rows, err := mysqlConn.Query(`
		select * from sy_item where FNumber like '%01.009%'`)
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

	fmt.Println(string(b))

	//db, err := sql.Open("mysql", "newflypig:jy.8215085@tcp(192.168.1.12:3306)/mysql?charset=utf8")
	//checkErr(err)

	// //插入数据
	// stmt, err := db.Prepare("INSERT userinfo SET username=?,departname=?,created=?")
	// checkErr(err)

	// res, err := stmt.Exec("码农", "研发部门", "2016-03-06")
	// checkErr(err)

	// id, err := res.LastInsertId()
	// checkErr(err)

	// fmt.Println(id)
	// //更新数据
	// stmt, err = db.Prepare("update userinfo set username=? where uid=?")
	// checkErr(err)

	// res, err = stmt.Exec("码农二代", id)
	// checkErr(err)

	// affect, err := res.RowsAffected()
	// checkErr(err)

	// fmt.Println(affect)

	//查询数据
	// rows, err := db.Query("SELECT host,user FROM user")
	// checkErr(err)

	// for rows.Next() {
	// 	var host, user *string
	// 	err = rows.Scan(&host, &user)
	// 	checkErr(err)
	// 	fmt.Println(*host, *user)
	// }

	//删除数据
	// stmt, err = db.Prepare("delete from userinfo where uid=?")
	// checkErr(err)

	// res, err = stmt.Exec(id)
	// checkErr(err)

	// affect, err = res.RowsAffected()
	// checkErr(err)

	// fmt.Println(affect)

	// defer db.Close()

}

func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}
