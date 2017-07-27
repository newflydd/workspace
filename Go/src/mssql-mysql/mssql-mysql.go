package main

import (
	"database/sql"
	"flag"
	"fmt"
	_ "github.com/denisenkom/go-mssqldb"
	_ "github.com/go-sql-driver/mysql"
	"log"
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

	/* 清空表 */
	mysqlStmt, err := mysqlConn.Prepare("delete from sy_item")
	checkErr("mysql prepare failed:", err)
	_, err = mysqlStmt.Exec()

	/* 迭代插入 */
	mysqlStmt, err = mysqlConn.Prepare("INSERT INTO sy_item(FItemID, FNumber, FUnitName, FName, FModel, FQty, FStockName) Values(?,?,?,?,?,?,?)")
	checkErr("mysql prepare failed:", err)

	defer mysqlStmt.Close()

	var FItemID *int
	var FQty *float32
	var FNumber, FUnitName, FName, FModel, FStockName *string
	for msRows.Next() {
		err = msRows.Scan(&FItemID, &FNumber, &FUnitName, &FName, &FModel, &FQty, &FStockName)
		checkErr("mssql scan failed:", err)

		if FModel == nil {
			var fmodel string = ""
			FModel = &fmodel
		}

		_, err = mysqlStmt.Exec(*FItemID, *FNumber, *FUnitName, *FName, *FModel, *FQty, *FStockName)
		checkErr("mysql exec failed:", err)
	}

	/* 跟新刷新时间 */
	mysqlStmt, err = mysqlConn.Prepare("update sy_system s set s.`value` = date_format(now(),'%Y-%m-%d %H:%i:%s') where s.`key` = 'update_time'")
	checkErr("mysql prepare failed:", err)
	_, err = mysqlStmt.Exec()
}

func checkErr(msg string, err error) {
	if err != nil {
		log.Fatal(msg, err)
	}
}
