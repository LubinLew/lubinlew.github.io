package main

import "fmt"

//匿名函数被直接赋值函数变量
var sum = func(a, b int) int {
	return a + b
}

//函数名作为函数返回值
func wrapper(op string) func(int, int) int {
	switch op {
	case "add":
		return func(a, b int) int {
			return a + b
		}
	case "sub":
		return func(a, b int) int {
			return a - b
		}
	default:
		return nil
	}
}

//匿名函数作为函数形参
func doprint(f func(int, int) int, a, b int) int {
	return f(a, b)
}

func main() {
	//匿名函数直接调用
	defer func() {
		if err := recover(); err != nil {
			fmt.Println(err)
		}
	}()

	sum(1, 2)

	doprint(func(x, y int) int {
		return x + y
	}, 1, 2)

	opFunc := wrapper("add")
	re := opFunc(2, 3)
	fmt.Printf("%d\n", re)
}
