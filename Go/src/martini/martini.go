package main

import (
	"fmt"
	"github.com/go-martini/martini"
	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/mysql"
	"github.com/martini-contrib/render"
	"html/template"
	"time"
)

var (
	WxAppID    string = "wxf35eb6c638780fbf"
	WxSecret   string = "007f2f4e18d0c0a42cabf09ca66c74dd"
	WxAppTitle string = "江苏赛洋"

	WxAccessToken        string = "" /* 微信AccessToken */
	WxAccessTokenGetTime int64  = 0  /* 最近一次获取微信AccessToken的时间戳 */
	WxJSTicket           string = "" /* 微信JSTicket */
	WxJSTicketGetTime    int64  = 0  /* 最近一次获得微信JSTicket的时间戳 */

	CommonMap map[string]interface{} = map[string]interface{}{
		"title": WxAppTitle,
	}
)

func main() {
	m := martini.Classic()

	//创建MySQL连接池
	mysqlConnPool, err := gorm.Open("mysql", "newflypig:jy.8215085@tcp(58.222.139.38:3306)/sywx?charset=utf8")
	checkErr(err)
	mysqlConnPool.DB().SetMaxIdleConns(10)
	mysqlConnPool.DB().SetMaxOpenConns(100) /* 设置最大连接数100 */
	//mysqlConnPool.LogMode(true)
	mysqlConnPool.SingularTable(true) /* 全局禁用复数表名 */
	defer mysqlConnPool.Close()

	/**
	 * martini 调用路由是使用reflect.Invoke 的方式调用的
	 * 因此注入连接池后，需要用的时候只需要在Handler参数中加入 *sql.DB 类型即可
	 */
	m.Map(mysqlConnPool)

	/* 使用render中间件，渲染模板 */
	m.Use(render.Renderer(render.Options{
		Funcs: []template.FuncMap{
			{
				"time": func(args ...interface{}) string {
					t1 := time.Unix(args[0].(int64), 0)
					return t1.Format(time.Stamp)
				},
			},
		},
	}))

	RouteMartini(m)

	m.Run()
}

func checkErr(err error) {
	if err != nil {
		fmt.Println(err)
	}
}
