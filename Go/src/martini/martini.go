package main

import (
	"encoding/xml"
	"fmt"
	"github.com/go-martini/martini"
	"io/ioutil"
	"models"
	"net/http"
	"time"
)

func main() {
	m := martini.Classic()

	/* 微信接入认证，一次性 */
	m.Get("/", func(req *http.Request) string {
		req.ParseForm()
		if req.Form["echostr"] != nil {
			return req.Form["echostr"][0]
		} else {
			return ""
		}
	})

	m.Post("/", func(req *http.Request) string {
		bodyBytes, err := ioutil.ReadAll(req.Body)
		checkErr(err)

		var receiveEvent = new(models.WxEvent)

		err = xml.Unmarshal(bodyBytes, receiveEvent)
		checkErr(err)

		fmt.Println(receiveEvent.ToUserName, receiveEvent.FromUserName, receiveEvent.Content)

		var responseEvent = models.WxEvent{}

		responseEvent.FromUserName = receiveEvent.ToUserName
		responseEvent.ToUserName = receiveEvent.FromUserName
		responseEvent.MsgType = "text"
		responseEvent.Content = "你好，你的消息我们已记录，请等待处理。"
		responseEvent.CreateTime = time.Now().Unix()

		repBytes, _ := xml.Marshal(responseEvent)
		return string(repBytes)
	})

	m.Run()
}

func checkErr(err error) {
	if err != nil {
		fmt.Println(err)
	}
}
