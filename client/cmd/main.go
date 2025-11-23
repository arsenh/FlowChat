package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"
	"time"
)

func main() {

	inputReader := bufio.NewReader(os.Stdin)

	serverAddr := "0.0.0.0:8081"

	conn, err := net.Dial("tcp", serverAddr)
	if err != nil {
		fmt.Printf("Error connecting to server: %v\n", err)
		return
	}
	defer conn.Close()

	fmt.Printf("Connected to TCP server at %s\n", serverAddr)

	remoteReader := bufio.NewReader(conn)

	for {
		fmt.Print(">>> ")
		input, _ := inputReader.ReadString('\n')
		input = strings.TrimSpace(input) + "\n"

		if input == "exit\n" {
			fmt.Println("Exiting ...")
			break
		}

		conn.SetWriteDeadline(time.Now().Add(5 * time.Second))

		_, err := conn.Write([]byte(input))
		if err != nil {
			fmt.Printf("Error sending data: %v\n", err)
			continue
		}

		serverData, err := remoteReader.ReadString('\n')
		if err != nil {
			fmt.Printf("Error reading from server: %v\n", err)
			continue
		}

		fmt.Printf("Server Response: %s\n", serverData)
	}
}
