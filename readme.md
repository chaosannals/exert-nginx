# exert-nginx

## Docker

### 模块编译 Docker

nginx 至少需要 pcre 和 zlib 库，所以需要先下好源码，如果开 SSL HTTPS 则需要 openssl 源码。

- /src 需要预先下载好需要的库的源码。
- /wks 为本项目目录。

```bash
docker build -t exert-nginx .
```

#### 编译容器

```bash
docker run -itd -v /host/workspace:/wks -v /host/source:/src -v /host/workspace/conf:/etc/nginx -w /src/nginx --name exert-nginx exert-nginx
```

#### 进入容器

```bash
docker exec -it exert-nginx /bin/bash
```

#### 容器内执行编译

```bash
./configure --with-compat --with-pcre=/src/pcre --with-zlib=/src/zlib --add-dynamic-module=/wks

make modules
```

编译后的模块文件在 /src/nginx/objs 里面。
