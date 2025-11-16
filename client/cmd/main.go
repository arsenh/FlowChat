package main

import (
	"fmt"
	"net"
	"time"
)

func main() {
	serverAddr := "localhost:8081"

	conn, err := net.Dial("tcp", serverAddr)

	if err != nil {
		fmt.Printf("Error connecting to server: %v\n", err)
	}

	defer conn.Close()

	fmt.Printf("Connected to TCP server at %s\n", serverAddr)

	message := "Hello to TCP Server from Go!"
	bytes := []byte(message)

	conn.SetWriteDeadline(time.Now().Add(5 * time.Second))

	n, err := conn.Write(bytes)
	if err != nil {
		fmt.Printf("Error sending data: %v\n", err)
	}

	fmt.Printf("Sent %d bytes\n", n)
}
