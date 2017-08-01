package main

import (
	"fmt"
	_ "github.com/go-sql-driver/mysql"
	"github.com/go-xorm/core"
	"github.com/go-xorm/xorm"
)

type User struct {
	Id   int64
	Name string `xorm:"varchar(25) notnull unique"`
}

func main1() {
	engine, err := xorm.NewEngine("mysql", "newflypig:jy.8215085@tcp(192.168.1.12:3306)/sywx?charset=utf8")
	if err != nil {
		fmt.Println(err)
	}

	defer engine.Close()

	engine.Logger().SetLevel(core.LOG_DEBUG)
	engine.ShowSQL(true)

	engine.CreateTables(&User{})

	engine.Insert(&User{Name: "test"})

	var user User
	engine.Get(&user)
	fmt.Println(user.Name)
}
