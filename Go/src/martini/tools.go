package main

import (
	"crypto/sha1"
	"fmt"
	"math/rand"
	"time"
)

//生成随机字符串
func GetRandomString(length int) string {
	str := "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
	bytes := []byte(str)
	result := []byte{}
	r := rand.New(rand.NewSource(time.Now().UnixNano()))
	for i := 0; i < length; i++ {
		result = append(result, bytes[r.Intn(len(bytes))])
	}
	return string(result)
}

/* SHA1散列 */
func Sha1Encrypt(str string) string {
	h := sha1.New()
	h.Write([]byte(str))
	estr := h.Sum(nil)
	return fmt.Sprintf("%x", estr)
}
