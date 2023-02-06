# make nginx in windows

## 项目配置

```bat
@echo off
chcp 65001

@rem 子模块源码（分支名带斜杠，以下命令不能成功，拆分多条切换）
git submodule add -b branches/stable-1.22 --depth 1 git@github.com:nginx/nginx.git nginxsrc

@rem 切子模块切 tag 到新分支（自定义命令行控制程序切换）
cd nginxsrc
git checkout -b v1.23.3 release-1.23.3

@rem 一条假设的命令（cat rm 找不到，环境应该是 windows ，导致 linux 命令找不到。）
D:\MSys2\usr\bin\env MSYSTEM=MSYS /usr/bin/bash -c auto/configure --with-cc=cl --with-debug  --prefix=  --conf-path=conf/nginx.conf  --pid-path=logs/nginx.pid  --http-log-path=logs/access.log  --error-log-path=logs/error.log  --sbin-path=nginx.exe  --http-client-body-temp-path=temp/client_body_temp  --http-proxy-temp-path=temp/proxy_temp  --http-fastcgi-temp-path=temp/fastcgi_temp  --http-scgi-temp-path=temp/scgi_temp  --http-uwsgi-temp-path=temp/uwsgi_temp  --with-cc-opt="-DFD_SETSIZE=1024 -D_CRT_SECURE_NO_WARNINGS"  --with-pcre=objs/lib/pcre-8.45  --with-zlib=objs/lib/zlib --with-openssl=objs/lib/openssl  --with-openssl-opt=no-asm  --with-http_ssl_module

@rem 需要在 Visual Studio 的工具链命令下执行 nmake
nmake
```

## 手动处理

```bat
@echo off
chcp 65001

@rem 克隆源码
git clone -b branches/stable-1.22 --depth 1 git@github.com:nginx/nginx.git nginxsrc

@rem 如果下载的 zip 包是没有 auto/configire 这个文件的，只有 git 拉取有，此时需要在 msys2 下用 autoconf 工具生成，会多出很多步骤，不推荐。

@rem 下载 pcre   zlib  openssl 源码 到 objs/lib 目录下
@rem 官方示例使用 pcre2 的示例，但是亲测不可行，还是使用 pcre 编译了。
```

```bash
# 在 msys2 环境下执行，缺啥装啥

# 修改 C编译参数 主要是去掉 \WX （该项强制警告以错误处理导致编译失败）
export CFLAGS="-O2 -W4 -nologo -MT"

# 通过 msys2 环境配置
auto/configure --with-cc=cl --prefix=  --conf-path=conf/nginx.conf  --pid-path=logs/nginx.pid  --http-log-path=logs/access.log  --error-log-path=logs/error.log  --sbin-path=nginx.exe  --http-client-body-temp-path=temp/client_body_temp  --http-proxy-temp-path=temp/proxy_temp  --http-fastcgi-temp-path=temp/fastcgi_temp  --http-scgi-temp-path=temp/scgi_temp  --http-uwsgi-temp-path=temp/uwsgi_temp  --with-cc-opt="-DFD_SETSIZE=1024 -D_CRT_SECURE_NO_WARNINGS"  --with-pcre=objs/lib/pcre-8.45  --with-zlib=objs/lib/zlib --with-openssl=objs/lib/openssl  --with-openssl-opt=no-asm  --with-http_ssl_module
```

```bat
@rem 在 Visual Studio 编译工具命令行执行
nmake
```