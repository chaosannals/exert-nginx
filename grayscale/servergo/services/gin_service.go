package services

import (
	"fmt"
	"log/slog"
	"net/http"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/samber/do"
	"golang.org/x/net/http2"
	"golang.org/x/net/http2/h2c"
)

type GinService struct {
	server *http.Server
	router *gin.Engine
	logger *slog.Logger
	conf   *ConfService
}

func NewGinService(i *do.Injector) (*GinService, error) {
	router := gin.New()
	logger := do.MustInvoke[*slog.Logger](i)
	conf := do.MustInvoke[*ConfService](i)

	router.Any("/", func(ctx *gin.Context) {
		logger.Info("hello")
		ctx.Writer.WriteString("hello")
		ctx.Status(http.StatusOK)
		ctx.Abort()
	})

	router.Any("/delay", func(ctx *gin.Context) {
		logger.Info("delay")
		time.Sleep(10 * time.Second)
		ctx.Writer.WriteString("delay")
		ctx.Status(http.StatusOK)
		ctx.Abort()
	})

	address := fmt.Sprintf("%s:%d", conf.Host, conf.Port)

	server := &http.Server{
		Addr:           address,
		Handler:        h2c.NewHandler(router.Handler(), &http2.Server{}),
		ReadTimeout:    30 * time.Second,
		WriteTimeout:   30 * time.Second,
		MaxHeaderBytes: 1000000,
	}

	return &GinService{
		server: server,
		router: router,
		logger: logger,
		conf:   conf,
	}, nil
}

func (i *GinService) Run() {
	i.logger.Info("start gin server")
	if err := i.server.ListenAndServe(); err == nil {
		i.logger.Error("serve failed", "error", err)
	}
}
