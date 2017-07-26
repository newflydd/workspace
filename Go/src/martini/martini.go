package main

import (
	"fmt"
	"github.com/go-martini/martini"
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
