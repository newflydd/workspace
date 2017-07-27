package main

import (
	"encoding/json"
	"encoding/xml"
	"fmt"
	"github.com/go-martini/martini"
	"github.com/martini-contrib/render"
	"io/ioutil"
	"models"
	"net/http"
	"time"
)

func RouteMartini(m *martini.ClassicMartini) {
	/* 微信接入认证，一次性 */
	m.Get("/", func(req *http.Request) string {
		req.ParseForm()
		if req.Form["echostr"] != nil {
			return req.Form["echostr"][0]
		} else {
			return ""
		}
	})

	//根目录，用来接收主体消息推送
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
		if receiveEvent.Event == "subscribe" {
			/* 用户第一次订阅，反馈欢迎信息 */
			responseEvent.Content = `江苏赛洋机电科技有限公司是一家专业从事数控机床配套产品的研发、制造、销售为一体的高新技术型企业、中国机床工具工业协会单位，公司创建于2001年。公司拥有成熟的研发技术、先进的生产设备、专业的检测仪器和可靠性的测试设备，可接受原厂委托设计制造以及原厂委托代工制造，并按照客户的要求提供多样个性化的设计产品。主要产品有控制面板、连接器、系统电缆线、继电器模板、波段开关、手持单元等。公司在专业设计方面已获得多项产品专利，并于2005年通过ISO9001-2000质量体系认证。`
		} else {
			responseEvent.Content = "你好，你的消息我们已记录，请等待处理。"
		}
		responseEvent.CreateTime = time.Now().Unix()

		repBytes, _ := xml.Marshal(responseEvent)
		return string(repBytes)
	})

	/* 使用render中间件，渲染模板页面，模板路径默认为/templates/ */
	m.Get("/abc.html", func(req *http.Request, r render.Render) {
		/* 获取微信传过来的code */
		req.ParseForm()
		if req.Form["code"] == nil {
			r.HTML(200, "error", "没有微信授权信息，请从微信App中访问本应用。")
			return
		}

		code := req.Form["code"][0]

		/* 用code换access_token和openid */
		resp, err := http.Get("https://api.weixin.qq.com/sns/oauth2/access_token?appid=" + WxAppID + "&secret=" + WxSecret + "&code=" + code + "&grant_type=authorization_code")
		checkErr(err)

		/* 解析微信返回的json，其中有access_token和openid */
		body, err := ioutil.ReadAll(resp.Body)
		checkErr(err)
		wxATOI := models.WxATOI{}
		err = json.Unmarshal(body, &wxATOI)
		checkErr(err)

		/* 通过access_token and openid 换取用户信息 */
		resp, err = http.Get("https://api.weixin.qq.com/sns/userinfo?access_token=" + wxATOI.AccessToken + "&openid=" + wxATOI.OpenId + "&lang=zh_CN")
		checkErr(err)

		/* 解析微信返回的json，其中有用户数据 */
		body, err = ioutil.ReadAll(resp.Body)
		checkErr(err)
		fmt.Println(string(body))
		wxUserInfo := models.WxUserInfo{}
		err = json.Unmarshal(body, &wxUserInfo)
		checkErr(err)

		/* 获取mysql中的数据跟新时间 */
		resp, err = http.Get("http://localhost:3000/api/getUpdateTime")
		checkErr(err)
		body, err = ioutil.ReadAll(resp.Body)
		checkErr(err)
		updateTime := string(body)
		fmt.Println(updateTime)

		defer resp.Body.Close()

		newmap := CommonMap
		newmap["code"] = req.Form["code"][0] /* 需要注入模板的各种变量 */
		newmap["openid"] = wxATOI.OpenId
		newmap["nickname"] = wxUserInfo.NickName
		newmap["headimgurl"] = wxUserInfo.HeadImgUrl
		newmap["updatetime"] = updateTime

		r.HTML(200, "abc", newmap)
		newmap["code"] = "bad value"
		newmap["openid"] = "bad value"
		newmap["nickname"] = "bad value"
		newmap["headimgurl"] = "bad value"
		newmap["updatetime"] = "bad value"
	})

	m.Group("/api", func(r martini.Router) {
		r.Get("/getStoreData", ApiGetStoreData)
		r.Get("/getUpdateTime", ApiGetUpdateTime)
	})
}
