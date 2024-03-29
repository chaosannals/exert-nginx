#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

// NGX_LOG_INFO 在 docker 下看不到 stdout 的输出
// NGX_LOG_ERR 可以在 docker 下看到 stderr 的输出

static char *ngx_http_grayngx(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_grayngx_handler(ngx_http_request_t *r);

/***************************************************************************
 * 周期处理程序区
 */

 /**
  * master 进程初始化阶段
  * 因为是通过 load_module 加载，所以此时 master 已经启动了，所以没有进入这个阶段。
  * 这个阶段必须集成编译到 nginx 里面才会触发，nginx.conf 动态加载不会触发。
  */
static ngx_int_t on_init_master(ngx_log_t *log) {
    ngx_log_error(NGX_LOG_INFO, log, 0, "grayngx on init master");
    return NGX_OK;
}

/**
 * 模块初始化阶段，从日志上看启动的时候被调用了 2 次。有点奇怪。
 */
static ngx_int_t on_init_module(ngx_cycle_t *cycle) {
    ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "grayngx on init module start");

    ngx_uint_t i;
    for (i = 0; cycle->modules[i]; i++) {
        if (cycle->modules[i]->init_module) {
            ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "grayngx on init module has init_module");
            // 如果执行下面的代码区再次调用 init_module 会造成无限死循环。
            // if (cycle->modules[i]->init_module(cycle) != NGX_OK) {
            //     return NGX_ERROR;
            // }
        }
    }

    ngx_log_error(NGX_LOG_ERR, cycle->log, 0, "grayngx on init module: %d", i);

    return NGX_OK;
}

/**
 * 进程初始化阶段
 * nginx.conf 里 worker_processes 的数量影响到这个被调用的次数。
 */
 static ngx_int_t on_init_process(ngx_cycle_t *cycle) {
    ngx_log_error(NGX_LOG_INFO, cycle->log, 0, "grayngx on init process");
    return NGX_OK;
 }

/**********************************************************************
 * 配置区
 */

/**
 * 模块提供的命令列表。
 * 
 */
static ngx_command_t ngx_http_grayngx_commands[] = {
    {
        ngx_string("grayngx"),                 /* directive */
        NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS, /* location context and takes no arguments*/
        ngx_http_grayngx,                      /* 命令配置安装函数 */
        0,                                   /* No offset. Only one context is supported. */
        0,                                   /* No offset when storing the module configuration on struct. */
        NULL                                 /**/
    },
    ngx_null_command /* 命令定义结尾 */
};

/**
 * 模块上下文
 * 
 */
static ngx_http_module_t ngx_http_grayngx_module_ctx = {
    NULL, /* preconfiguration */
    NULL, /* postconfiguration */

    NULL, /* create main configuration */
    NULL, /* init main configuration */

    NULL, /* create server configuration */
    NULL, /* merge server configuration */

    NULL, /* create location configuration */
    NULL  /* merge location configuration */
};

/**
 * 这个结构会被 Nginx 找到并读取相关配置。
 * 
 */
ngx_module_t ngx_http_grayngx_module = {
    NGX_MODULE_V1,
    &ngx_http_grayngx_module_ctx, /* module context */
    ngx_http_grayngx_commands,    /* module directives */
    NGX_HTTP_MODULE,            /* module type */
    on_init_master,                       /* init master */
    on_init_module,                       /* init module */
    on_init_process,                /* init process */
    NULL,                       /* init thread */
    NULL,                       /* exit thread */
    NULL,                       /* exit process */
    NULL,                       /* exit master */
    NGX_MODULE_V1_PADDING       /**/
};

/****************************************************************************
 * 命令定义区
 */

/**
 * 初始化模块的命令。
 * 
 */
static char *ngx_http_grayngx(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf; /* pointer to core location configuration */

    /* 安装命令执行方法。 */
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_grayngx_handler;

    return NGX_CONF_OK;
} /* ngx_http_grayngx */

static u_char ngx_grayngx[] = "grayngx-nginx";

/**
 * 命令被调用时执行的方法。
 * 
 */
static ngx_int_t ngx_http_grayngx_handler(ngx_http_request_t *r)
{
    ngx_buf_t *b;
    ngx_chain_t out;

    ngx_log_error(NGX_LOG_INFO, r->connection->log, 0, "on grayngx command run.");

    /* 设置 HTTP Content-Type 报首信息。 */
    r->headers_out.content_type.len = sizeof("text/plain") - 1;
    r->headers_out.content_type.data = (u_char *)"text/plain";

    /* 给 响应内容 分配 Buffer */
    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

    /* 插入 buffer 链. */
    out.buf = b;
    out.next = NULL; /* just one buffer */

    b->pos = ngx_grayngx; /* 指定内存首地址 */
    b->last = ngx_grayngx + sizeof(ngx_grayngx) - 1; /* 指定内存末地址 */
    b->memory = 1;                               /* 指定内存只读模式 */
    b->last_buf = 1;                             /* there will be no more buffers in the request */

    /* 设置 响应状态 */
    r->headers_out.status = NGX_HTTP_OK; /* 200 status code */
    /* 设置 响应头 Content-Length */
    r->headers_out.content_length_n = sizeof(ngx_grayngx) - 1;
    ngx_http_send_header(r); /* 设置起效 */

    /* 输出内容体 和 返回状态码 到 filter 链. */
    return ngx_http_output_filter(r, &out);
} /* ngx_http_grayngx_handler */