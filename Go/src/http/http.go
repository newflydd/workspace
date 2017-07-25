// main.go
package main

import (
	"fmt"
	"log"
	"net/http"
)

// 处理主页请求
func index(w http.ResponseWriter, r *http.Request) {
	// 向客户端写入内容
	fmt.Fprintf(w, "Hello World!")
}

func main() {
	http.HandleFunc("/", index)              //设置访问的路由
	err := http.ListenAndServe(":9090", nil) //设置监听的端口
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
