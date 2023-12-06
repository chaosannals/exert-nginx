package main

import (
	"github.com/chaosannals/exert-nginx/grayscale/services"
	"github.com/samber/do"
)

func main() {
	// 容器
	injector := do.New()
	defer injector.Shutdown()

	// 配置
	do.Provide(injector, services.NewConfService)

	// 日志
	do.Provide(injector, services.NewLogger)

	// gin
	do.Provide(injector, services.NewGinService)

	// 启动
	server := do.MustInvoke[*services.GinService](injector)
	server.Run()
}
