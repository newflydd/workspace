package main

import (
	"fmt"
	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/mysql"
	"time"
)

type SyCheckOrder struct {
	CheckOrderId int64 `gorm:"primary_key"`
	CreatedAt    time.Time
	UserId       int64
	StockName    string
	State        int8
}

type Product struct {
	gorm.Model
	Code  string
	Price uint
}

func main() {
	db, err := gorm.Open("mysql", "newflypig:jy.8215085@tcp(192.168.1.12:3306)/sywx?charset=utf8&parseTime=True&loc=Local")
	if err != nil {
		fmt.Println("failed to connect database")
		fmt.Println(err)
	}
	db.LogMode(true)

	fmt.Println("connect database success!")

	product := Product{}
	db.AutoMigrate(&Product{})

	//product.Code = "BAC"
	//product.Price = 18
	//db.Create(&product)

	db.First(&product)

	fmt.Println(product.Code)

	/* 创建表格 */
	//db.AutoMigrate(&SyCheckOrder{})

	//syCheckOrder := SyCheckOrder{}
	/* insert */
	// syCheckOrder = SyCheckOrder{CreatedAt: time.Now(), UserId: 2, StockName: "四楼仓库", State: 0}
	// db.Create(&syCheckOrder)

	/* select */
	var syCheckOrder SyCheckOrder
	db.First(&syCheckOrder)

	if syCheckOrder.StockName == "" {
		fmt.Println("not loaded")
	} else {
		fmt.Println(syCheckOrder.StockName)
	}

}
