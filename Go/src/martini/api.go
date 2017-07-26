package main

import (
	"github.com/go-martini/martini"
)

func ApiGetStoreData(params martini.Params) string {

	return `{"id":"` + params["thing_id"] + `", "store_num":100}`
}
