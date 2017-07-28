package main

import (
	"database/sql"
	"flag"
	"fmt"
	_ "github.com/denisenkom/go-mssqldb"
	_ "github.com/go-sql-driver/mysql"
	"log"
	"models"
)

var (
	debug         = flag.Bool("debug", true, "enable debugging")
	password      = flag.String("password", "admin#", "the database password")
	port     *int = flag.Int("port", 1433, "the database port")
	server        = flag.String("server", "192.168.1.6", "the database server")
	user          = flag.String("user", "sa", "the database user")
	database      = flag.String("database", "AIS20121212134113", "the database name")
)

func main() {
	flag.Parse()

	if *debug {
		fmt.Printf(" password:%s\n", *password)
		fmt.Printf(" port:%d\n", *port)
		fmt.Printf(" server:%s\n", *server)
		fmt.Printf(" user:%s\n", *user)
		fmt.Printf(" database:%s\n", *database)
	}

	msConnString := fmt.Sprintf("server=%s;user id=%s;password=%s;port=%d;database=%s;encrypt=disable;", *server, *user, *password, *port, *database)
	if *debug {
		fmt.Printf(" MSConnString:%s\n", msConnString)
	}

	msConn, err := sql.Open("mssql", msConnString)
	checkErr("Open mssql connection failed:", err)
	defer msConn.Close()

	msStmt, err := msConn.Prepare(`
        select item.FItemID, item.FNumber,  mu.FName, item.FName, item.FModel, v.FQty, s.FName  
            from dbo.t_ICItem item                                                              
            join dbo.t_measureUnit mu on mu.FMeasureUnitId = item.FStoreUnitId                  
            join dbo.ICInventory v on v.FItemID = item.FItemID                                  
            join dbo.t_stock s on s.FItemID = v.FStockID                                        
    `)
	checkErr("prepare mssql failed:", err)
	defer msStmt.Close()

	msRows, err := msStmt.Query()
	checkErr("mssql query failed:", err)

	mysqlConn, err := sql.Open("mysql", "newflypig:jy.8215085@tcp(192.168.1.12:3306)/sywx?charset=utf8")
	checkErr("Open mssql connection failed:", err)
	defer mysqlConn.Close()

	mysqlConn.SetMaxOpenConns(4500)

	/* 清空表 */
	mysqlStmt, err := mysqlConn.Prepare("delete from sy_item")
	checkErr("mysql prepare failed:", err)
	_, err = mysqlStmt.Exec()

	/* 迭代插入 */
	mysqlStmt, err = mysqlConn.Prepare("INSERT INTO sy_item(FItemID, FNumber, FUnitName, FName, FModel, FQty, FStockName) Values(?,?,?,?,?,?,?)")
	checkErr("mysql prepare failed:", err)

	defer mysqlStmt.Close()

	/* 切片，相当于动态数组 */
	fitems := []models.K3ItemPoint{}

	for msRows.Next() {
		fitem := models.K3ItemPoint{}

		err = msRows.Scan(&fitem.FItemID, &fitem.FNumber, &fitem.FUnitName, &fitem.FName, &fitem.FModel, &fitem.FQty, &fitem.FStockName)
		checkErr("mssql scan failed:", err)

		if fitem.FModel == nil {
			blankStr := ""
			fitem.FModel = &blankStr
		}

		fitems = append(fitems, fitem)
	}

	fmt.Printf("已读取 %d 条记录，准备插入MySQL数据库。", len(fitems))

	/* 遍历切片，开启协程处理数据库插入 */
	for i := 0; i < len(fitems); i++ {
		go insertIntoMysql(mysqlStmt, fitems[i])
	}

	/* 跟新刷新时间 */
	mysqlStmt, err = mysqlConn.Prepare("update sy_system s set s.`value` = date_format(now(),'%Y-%m-%d %H:%i:%s') where s.`key` = 'update_time'")
	checkErr("mysql prepare failed:", err)
	_, err = mysqlStmt.Exec()
}

func insertIntoMysql(mysqlStmt *sql.Stmt, fitem models.K3ItemPoint) {
	_, err := mysqlStmt.Exec(*fitem.FItemID, *fitem.FNumber, *fitem.FUnitName, *fitem.FName, *fitem.FModel, *fitem.FQty, *fitem.FStockName)
	checkErr("mysql exec failed:", err)
}

func checkErr(msg string, err error) {
	if err != nil {
		log.Fatal(msg, err)
	}
}
