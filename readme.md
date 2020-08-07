# exert-nginx

## 原生执行

```bash
./configure --with-compat --add-dynamic-module=/wks
make modules
```

## Docker

- /src 需要预先下载好需要的库的源码。
- /wks 为本项目目录。

```bash
docker build -t exert-nginx .
```

### 生成 makefile

nginx 至少需要 pcre 和 zlib 所以需要先下好源码，如果开 SSL HTTPS 则需要 openssl 源码。

```bash
docker run -it -v /host/workspace:/wks -v /host/source:/src -w /src/nginx --entrypoint ./configure exert-nginx ./configure --with-compat --with-pcre=/src/pcre --with-zlib=/src/zlib --add-dynamic-module=/wks
```

### 执行 make

```bash
docker run -it -v /host/workspace:/wks -v /host/source:/src -w /src/nginx --name exert-nginx exert-nginx
```
