package main

import (
	"database/sql"
	"fmt"
	"github.com/go-martini/martini"
	_ "github.com/go-sql-driver/mysql"
	"github.com/martini-contrib/render"
	"html/template"
	"time"
)

var (
	WxAppID    string = "wxf35eb6c638780fbf"
	WxSecret   string = "007f2f4e18d0c0a42cabf09ca66c74dd"
	WxAppTitle string = "江苏赛洋"

	CommonMap map[string]interface{} = map[string]interface{}{
		"title": WxAppTitle,
	}
)

func main() {
	m := martini.Classic()

	//创建MySQL连接池
	mysqlConnPool, err := sql.Open("mysql", "newflypig:jy.8215085@tcp(58.222.139.38:3306)/sywx?charset=utf8")
	checkErr(err)
	mysqlConnPool.SetMaxIdleConns(10) /* 设置最大连接数10 */
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
				"title": func(args ...interface{}) string {
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
