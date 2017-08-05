package main

import (
	"encoding/json"
	"encoding/xml"
	"fmt"
	"github.com/go-martini/martini"
	"github.com/jinzhu/gorm"
	"github.com/martini-contrib/render"
	"io/ioutil"
	"log"
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

		var receiveEvent = new(WxEvent)

		err = xml.Unmarshal(bodyBytes, receiveEvent)
		checkErr(err)

		fmt.Println(receiveEvent.ToUserName, receiveEvent.FromUserName, receiveEvent.Event, receiveEvent.Content)

		var responseEvent = WxEvent{}

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
	m.Get("/abc.html", authorize, checkHasBind, func(r render.Render, syUser *SyUser, log *log.Logger, mysqlConn *gorm.DB) {
		/* 获取mysql中的数据跟新时间 */
		updateTime := ApiGetUpdateTime(mysqlConn)

		/* 构造JSTicket签名 */
		noncestr := GetRandomString(16)
		jsapi_ticket := getJSTicket()
		timestamp := time.Now().Unix()
		url := "http://www.codingcrafts.com/abc.html?code=" + syUser.Code + "&state=0"
		signatureStr := fmt.Sprintf("jsapi_ticket=%s&noncestr=%s&timestamp=%d&url=%s", jsapi_ticket, noncestr, timestamp, url)
		signature := Sha1Encrypt(signatureStr)

		log.Println("user:" + syUser.Name + " into search page@" + time.Now().Format("2006-01-02 15:04:05"))

		newmap := CommonMap
		newmap["name"] = syUser.Name
		newmap["updatetime"] = updateTime
		newmap["timestamp"] = timestamp
		newmap["nonceStr"] = noncestr
		newmap["signature"] = signature

		r.HTML(200, "abc", newmap)
	})

	/* 仓库盘点页面 */
	m.Get("/stockcheck.html", authorize, checkHasBind, func(r render.Render, syUser *SyUser, log *log.Logger, mysqlConn *gorm.DB) {
		/* 获取mysql中的数据跟新时间 */
		updateTime := ApiGetUpdateTime(mysqlConn)

		/* 构造JSTicket签名 */
		noncestr := GetRandomString(16)
		jsapi_ticket := getJSTicket()
		timestamp := time.Now().Unix()
		url := "http://www.codingcrafts.com/stockcheck.html?code=" + syUser.Code + "&state=0"
		signatureStr := fmt.Sprintf("jsapi_ticket=%s&noncestr=%s&timestamp=%d&url=%s", jsapi_ticket, noncestr, timestamp, url)
		signature := Sha1Encrypt(signatureStr)

		log.Println("user:" + syUser.Name + " into search page@" + time.Now().Format("2006-01-02 15:04:05"))

		newmap := CommonMap
		newmap["name"] = syUser.Name
		newmap["updatetime"] = updateTime
		newmap["timestamp"] = timestamp
		newmap["nonceStr"] = noncestr
		newmap["signature"] = signature
		r.HTML(200, "stockcheck", newmap)
	})

	/* 账号绑定页面 */
	m.Get("/register.html", authorize, checkHasBindToError, func(r render.Render, syUser *SyUser) {
		newmap := CommonMap
		newmap["openid"] = syUser.Openid
		newmap["nickname"] = syUser.NickName

		log.Println("wxuser:" + syUser.NickName + " into register page@" + time.Now().Format("2006-01-02 15:04:05"))

		r.HTML(200, "register", newmap)
	})

	/* 对整个API系列的请求实行验证中间件，这里中间件相当于拦截器，在中间件中可以使用c.Next()提前执行后面的handler，从而实现后置逻辑 */
	m.Group("/api", func(r martini.Router) {
		r.Get("/getStoreData", ApiGetStoreData)   /* 向服务器发送库存查询请求 */
		r.Post("/register", ApiRegister)          /* 向服务器发送账号绑定请求 */
		r.Get("/getStockNames", ApiGetStockNames) /* 向服务器请求仓库名列表 */
	})
}

func authorize(req *http.Request, r render.Render, c martini.Context) {
	/* 获取微信传过来的code */
	req.ParseForm()
	if req.Form["code"] == nil {
		r.HTML(200, "error", "没有微信授权信息，请从微信App中访问本应用。")
		return
	}

	/* 用code换access_token和openid */
	resp, err := http.Get("https://api.weixin.qq.com/sns/oauth2/access_token?appid=" + WxAppID + "&secret=" + WxSecret + "&code=" + req.Form["code"][0] + "&grant_type=authorization_code")
	checkErr(err)

	/* 解析微信返回的json，其中有access_token和openid */
	body, err := ioutil.ReadAll(resp.Body)
	checkErr(err)
	wxAtoiResp := WxAtoiResp{}
	err = json.Unmarshal(body, &wxAtoiResp)
	checkErr(err)

	/* 通过access_token and openid 换取用户信息 */
	resp, err = http.Get("https://api.weixin.qq.com/sns/userinfo?access_token=" + wxAtoiResp.AccessToken + "&openid=" + wxAtoiResp.OpenId + "&lang=zh_CN")
	checkErr(err)

	/* 解析微信返回的json，其中有用户数据 */
	body, err = ioutil.ReadAll(resp.Body)
	checkErr(err)
	syUser := SyUser{}
	err = json.Unmarshal(body, &syUser)
	checkErr(err)

	syUser.Code = req.Form["code"][0]

	c.Map(&syUser)
}

func getAccessToken() string {
	nowTime := time.Now().Unix()
	/* 如果距离上次申请AccessToken不足7000秒，则直接返回内存中的AccessToken */
	if nowTime-WxAccessTokenGetTime < 7000 {
		return WxAccessToken
	}

	resp, err := http.Get(`https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=` + WxAppID + `&secret=` + WxSecret)
	checkErr(err)

	body, err := ioutil.ReadAll(resp.Body)
	checkErr(err)

	wxAccessTokenResp := WxAccessTokenResp{}
	err = json.Unmarshal(body, &wxAccessTokenResp)
	if err != nil {
		fmt.Println("get access_token error.")
		fmt.Println(err)
		fmt.Println(string(body))
		return ""
	}

	WxAccessToken = wxAccessTokenResp.AccessToken
	WxAccessTokenGetTime = nowTime

	return WxAccessToken
}

func getJSTicket() string {
	nowTime := time.Now().Unix()

	/* 如果距离上次申请JSTicket不足7000秒，则直接返回内存中的AccessToken */
	if nowTime-WxJSTicketGetTime < 7000 {
		return WxJSTicket
	}

	accessToken := getAccessToken()

	if accessToken == "" {
		return ""
	}

	resp, err := http.Get(`https://api.weixin.qq.com/cgi-bin/ticket/getticket?access_token=` + accessToken + `&type=jsapi`)
	checkErr(err)

	body, err := ioutil.ReadAll(resp.Body)
	checkErr(err)

	wxJSTicketResp := WxJSTicketJsonResp{}
	err = json.Unmarshal(body, &wxJSTicketResp)

	if err != nil || wxJSTicketResp.ErrCode != 0 {
		fmt.Println("get js_ticket error.")
		return ""
	}

	WxJSTicket = wxJSTicketResp.Ticket
	WxJSTicketGetTime = nowTime

	return WxJSTicket
}
