# exert-nginx

## Docker

### 模块编译 Docker

- /src 为插件源码。
- config 文件必须是 LF 格式，Windows 下 CRLF 会导致错误，如果被 GIT 设置为 CRLF 格式，自行修改成 LF。
- nginx 至少需要 pcre 和 zlib 库，所以需要先下好源码，如果开 SSL HTTPS 则需要 openssl 源码。
- 通过修改 dockerfile 来添加更多库。


## 常用命令

```bash
# 修改配置后测试并重载配置
nginx -t && nginx -s reload
```