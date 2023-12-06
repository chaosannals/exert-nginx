# 灰度发布 demo

## 开发

```bash
# 构建 nginx 扩展镜像生成
docker build -f ./grayngx/dockerfile -t grayngxbuilder ./grayngx
```

```bash
# 修改代码后重新上 nginx

# 下 nginx
docker compose down grayscale-nginx --rmi local

# 上 nginx
docker compose up grayscale-nginx -d

# 执行 confgo
docker exec -it grayscale-nginx /confgo
```

##  confgo 操作大概步骤

```conf
# 修改 upstream 注释掉
upstream goserver {
    server grayscale-server-go-1:40040 weight=4;
    #server grayscale-server-go-2:40040 weight=4;
}
```

```bash
# 在 NGINX 容器内执行
# 会等待请求结束：
#   HTTP 问题不大
#   websocket 和 TCP UDP 下不知道断开时机，导致回收时间太长。
nginx -s reload
```