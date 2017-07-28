package main

import (
	"fmt"
	//"github.com/golang/example/stringutil"
	"bufio"
	"flag"
	"math"
	"math/rand"
	"os"
	"stringutil"
)

func sum(a []int) (sum int, length int) {
	sum = 0
	for _, c := range a {
		sum += c
	}
	return sum, len(a)
}

func main() {
	defer close()

	flag.Parse()
	fmt.Printf("hello world\n")
	fmt.Printf(stringutil.Reverse("中国") + "\n")
	fmt.Printf("%d\n", flag.NArg())

	a := []int{1, 2, 3, 4}
	summary, length := sum(a)
	fmt.Printf("%d,%d\n", summary, length)
	summary, length = sum(a)
	fmt.Printf("%d,%d\n", summary, length)

	fmt.Println(math.Pi)

	reader := bufio.NewReader(os.Stdin)
	for {
		data, _, _ := reader.ReadLine()
		command := string(data)
		switch command {
		case "stop":
			return
		default:
			fmt.Println(command, " ", rand.Intn(10))
		}
	}
}

func close() {
	fmt.Println("defer test")
}
