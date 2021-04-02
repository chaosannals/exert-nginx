FROM nginx:1.19.9 AS builder

COPY ./src /src

RUN apt -y update && \
    apt -y upgrade && \
    apt -y install gcc g++ make wget && \
    mkdir /wks && \
    cd /wks && \
    wget http://nginx.org/download/nginx-1.19.9.tar.gz && \
    tar -zxvf nginx-1.19.9.tar.gz && \
    wget https://ftp.pcre.org/pub/pcre/pcre-8.44.tar.gz && \
    tar -zxvf pcre-8.44.tar.gz && \
    wget https://www.zlib.net/zlib-1.2.11.tar.gz && \
    tar -zxvf zlib-1.2.11.tar.gz && \
    cd /wks/nginx-1.19.9 && \
    ./configure \
        --with-compat \
        --with-pcre=/wks/pcre-8.44 \
        --with-zlib=/wks/zlib-1.2.11 \
        --add-dynamic-module=/src \
        && \
    make modules



FROM nginx:1.19.9

COPY --from=builder /wks/nginx-1.19.9/objs /mod
