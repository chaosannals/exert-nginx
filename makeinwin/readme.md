# make nginx in windows

关于 nginx 的 windows 编译没有太多规律可循。我 2015年 2017年 2020年 2023年编译，每次编译 nginx 出现的问题都不同。
基本上的思路是用 nginx -V 查看官方编译的信息，之后报了什么错就去网上查，大概问题分成以下几类：

1. 因为使用 Msys2 生成 makefile 所以有些 .obj 后缀是 .o 的，需要手动修改生成的 makefile
2. 生成的 cl 命令使用了老的命令（例如： -o output/path.obj 指定 obj 文件输出改用 /Fo"output/path.obj"）
3. 依赖的 lib 少了。（例：缺少系统的 User32.lib 链接参数）
4. cl 配置出问题。（例：启用 \WX 导致警告被当作错误。）
5. git 拉取版本 和 官网 zip 下载源码不同， zip 版本要用 autoconf 生成 configure ，才能用 configure 生成 makefile
6. ico 文件没有链接进 exe ，不过这个不影响使用。(例：生成的 makefile 缺少把 src/os/win32/nginx.rc 生成 src/os/win32/nginx.res 并添加到链接里面)。
7. 使用和官方的编译工具版本一致（VC++这些，应该可以少踩坑）
8. 依赖的库使用和官方版本参数一致的版本（openssl版本选用一样的，应该可以少踩坑）
9. 多次编译失败修改后成功的，之前步骤生成了文件，之后改动后又通过，但是清理后在编译可能失败。

注：以下参数仅供参考，实际编译时因为版本和编译工具依赖库使用不同，会出现各种问题。

```bat
@rem 查看 nginx 编译参数
nginx -V

@rem 生成 rc 生成 res 文件
rc src/os/win32/nginx.rc
```

```bash
auto/configure --with-cc=cl --builddir=objs --with-debug --prefix= --conf-path=conf/nginx.conf --pid-path=logs/nginx.pid --http-log-path=logs/access.log --error-log-path=logs/error.log --sbin-path=nginx.exe --http-client-body-temp-path=temp/client_body_temp --http-proxy-temp-path=temp/proxy_temp --http-fastcgi-temp-path=temp/fastcgi_temp --http-scgi-temp-path=temp/scgi_temp --http-uwsgi-temp-path=temp/uwsgi_temp --with-cc-opt=-DFD_SETSIZE=1024 --with-pcre=objs/lib/pcre2-10.39 --with-zlib=objs/lib/zlib --with-http_v2_module --with-http_realip_module --with-http_addition_module --with-http_sub_module --with-http_dav_module --with-http_stub_status_module --with-http_flv_module --with-http_mp4_module --with-http_gunzip_module --with-http_gzip_static_module --with-http_auth_request_module --with-http_random_index_module --with-http_secure_link_module --with-http_slice_module --with-mail --with-stream --with-openssl=objs/lib/openssl --with-openssl-opt='no-asm no-tests -D_WIN32_WINNT=0x0501' --with-http_ssl_module --with-mail_ssl_module --with-stream_ssl_module
```


- msys2
- [gnuwin32 的 sed](https://gnuwin32.sourceforge.net/packages/sed.htm) nmake 执行 nginx 构建依赖 (直接下载 Complete 是安装包，如果之前编译失败到这里要重启命令行。也可以自己逐个下载放到已有PATH 的路径下，这样不用重启命令行)
- [gnuwin32 的 libintl3](https://gnuwin32.sourceforge.net/packages/libintl.htm) sed 依赖
- [gnuwin32 的 regex2](https://gnuwin32.sourceforge.net/packages/regex.htm) sed 依赖
- [gnuwin32 的 libiconv](https://gnuwin32.sourceforge.net/packages/libiconv.htm) sed 依赖

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
auto/configure --with-cc=cl --prefix=  --conf-path=conf/nginx.conf  --pid-path=logs/nginx.pid  --http-log-path=logs/access.log  --error-log-path=logs/error.log  --sbin-path=nginx.exe  --http-client-body-temp-path=temp/client_body_temp  --http-proxy-temp-path=temp/proxy_temp  --http-fastcgi-temp-path=temp/fastcgi_temp  --http-scgi-temp-path=temp/scgi_temp  --http-uwsgi-temp-path=temp/uwsgi_temp  --with-cc-opt="-DFD_SETSIZE=1024"  --with-pcre=objs/lib/pcre-8.45  --with-zlib=objs/lib/zlib --with-openssl=objs/lib/openssl  --with-openssl-opt=no-asm  --with-http_ssl_module
```

```bat
@rem 在 Visual Studio 编译工具命令行执行
nmake
```


## 通过 minw

```bash
# 配置
auto/configure --with-cc=x86_64-w64-mingw32-gcc.exe --prefix=  --conf-path=conf/nginx.conf  --pid-path=logs/nginx.pid  --http-log-path=logs/access.log  --error-log-path=logs/error.log  --sbin-path=nginx.exe  --http-client-body-temp-path=temp/client_body_temp  --http-proxy-temp-path=temp/proxy_temp  --http-fastcgi-temp-path=temp/fastcgi_temp  --http-scgi-temp-path=temp/scgi_temp  --http-uwsgi-temp-path=temp/uwsgi_temp  --with-cc-opt="-g -O2"  --with-pcre=objs/lib/pcre-8.45  --with-zlib=objs/lib/zlib --with-openssl=objs/lib/openssl  --with-openssl-opt=no-asm  --with-http_ssl_module

# 构建
mingw32-make
```
