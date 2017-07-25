package main

import (
	"database/sql"
	"flag"
	"fmt"
	"log"

	_ "github.com/denisenkom/go-mssqldb"
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

	connString := fmt.Sprintf("server=%s;user id=%s;password=%s;port=%d;database=%s;encrypt=disable;", *server, *user, *password, *port, *database)
	if *debug {
		fmt.Printf(" connString:%s\n", connString)
	}
	conn, err := sql.Open("mssql", connString)
	if err != nil {
		log.Fatal("Open connection failed:", err.Error())
	}
	defer conn.Close()

	err = conn.Ping()
	if err != nil {
		log.Fatal("Ping failed:", err)
	}

	stmt, err := conn.Prepare(`
        select item.FItemID, item.FNumber,  mu.FName, item.FName, item.FModel, v.FQty, s.FName  
            from dbo.t_ICItem item                                                              
            join dbo.t_measureUnit mu on mu.FMeasureUnitId = item.FStoreUnitId                  
            join dbo.ICInventory v on v.FItemID = item.FItemID                                  
            join dbo.t_stock s on s.FItemID = v.FStockID                                        
    `)
	if err != nil {
		log.Fatal("Prepare failed:", err.Error())
	}
	defer stmt.Close()

	rows, err := stmt.Query()

	var FItemID *int
	var FQty *float32
	var FNumber, FUnitName, FName, FModel, FStockName *string
	for rows.Next() {
		err = rows.Scan(&FItemID, &FNumber, &FUnitName, &FName, &FModel, &FQty, &FStockName)
		if err != nil {
			log.Fatal("Scan failed:", err.Error())
		}

		if FModel == nil {
			fmt.Printf("%d-%s()\n", *FItemID, *FName)
		} else {
			//fmt.Printf("%d-%s(%s)\n", *FItemID, *FName, *FModel)
		}
	}

	fmt.Printf("bye\n")
}
