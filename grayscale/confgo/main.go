package main

import (
	"fmt"
	"math/rand"
	"os"
	"os/exec"
	"regexp"
	"strings"
)

var servers []string

func init() {
	servers = []string{
		"server grayscale-server-go-1:40040 weight=4;",
		"server grayscale-server-go-2:40040 weight=4;",
		"server grayscale-server-go-3:40040 weight=4;",
		"server grayscale-server-go-4:40040 weight=4;",
		"server grayscale-server-go-5:40040 weight=4;",
		"server grayscale-server-go-6:40040 weight=4;",
	}
}

func replaceConf(path string) error {
	content, err := os.ReadFile(path)
	if err != nil {
		return err
	}

	pattern := regexp.MustCompile(`upstream\s+goserver\s*\{(.|\n|\r)+?\}`)
	found := pattern.Find(content)

	fmt.Printf("match: %s\n", string(found))

	end := 1 + rand.Intn(len(servers)-1)
	newServers := strings.Join(servers[:end], "\n")
	newContent := fmt.Sprintf("upstream goserver {\n %s \n}", newServers)
	fmt.Printf("new: %s\n", newContent)

	return nil
}

func main() {
	fmt.Println(os.Args[0])

	var confPath string

	if len(os.Args) > 1 {
		confPath = os.Args[1]
	} else {
		confPath = "/etc/nginx/nginx.conf"
	}

	fmt.Printf("replace conf: %s\n", confPath)
	if err := replaceConf(confPath); err != nil {
		fmt.Printf("replace error: %s", err.Error())
	}

	fmt.Println("nginx reload")
	cmd := exec.Command("nginx", "-s", "reload")
	if err := cmd.Run(); err != nil {
		fmt.Printf("error: %s", err.Error())
	}
}
