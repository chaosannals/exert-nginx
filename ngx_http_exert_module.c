#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static char *ngx_http_exert(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_exert_handler(ngx_http_request_t *r);

/**
 * 模块提供的命令列表。
 * 
 */
static ngx_command_t ngx_http_exert_commands[] = {
    {
        ngx_string("exert"),                 /* directive */
        NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS, /* location context and takes no arguments*/
        ngx_http_exert,                      /* 命令配置安装函数 */
        0,                                   /* No offset. Only one context is supported. */
        0,                                   /* No offset when storing the module configuration on struct. */
        NULL                                 /**/
    },
    ngx_null_command /* command termination */
};

/**
 * 模块上下文
 * 
 */
static ngx_http_module_t ngx_http_exert_module_ctx = {
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
ngx_module_t ngx_http_exert_module = {
    NGX_MODULE_V1,
    &ngx_http_exert_module_ctx, /* module context */
    ngx_http_exert_commands,    /* module directives */
    NGX_HTTP_MODULE,            /* module type */
    NULL,                       /* init master */
    NULL,                       /* init module */
    NULL,                       /* init process */
    NULL,                       /* init thread */
    NULL,                       /* exit thread */
    NULL,                       /* exit process */
    NULL,                       /* exit master */
    NGX_MODULE_V1_PADDING       /**/
};

/**
 * 初始化模块的命令。
 * 
 */
static char *ngx_http_exert(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf; /* pointer to core location configuration */

    /* 安装命令执行方法。 */
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_exert_handler;

    return NGX_CONF_OK;
} /* ngx_http_exert */

static u_char ngx_exert[] = "exert-nginx";

/**
 * 命令被调用时执行的方法。
 * 
 */
static ngx_int_t ngx_http_exert_handler(ngx_http_request_t *r)
{
    ngx_buf_t *b;
    ngx_chain_t out;

    /* 设置 HTTP Content-Type 报首信息。 */
    r->headers_out.content_type.len = sizeof("text/plain") - 1;
    r->headers_out.content_type.data = (u_char *)"text/plain";

    /* Allocate a new buffer for sending out the reply. */
    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

    /* Insertion in the buffer chain. */
    out.buf = b;
    out.next = NULL; /* just one buffer */

    b->pos = ngx_exert;                          /* first position in memory of the data */
    b->last = ngx_exert + sizeof(ngx_exert) - 1; /* last position in memory of the data */
    b->memory = 1;                               /* content is in read-only memory */
    b->last_buf = 1;                             /* there will be no more buffers in the request */

    /* Sending the headers for the reply. */
    r->headers_out.status = NGX_HTTP_OK; /* 200 status code */
    /* Get the content length of the body. */
    r->headers_out.content_length_n = sizeof(ngx_exert) - 1;
    ngx_http_send_header(r); /* Send the headers */

    /* Send the body, and return the status code of the output filter chain. */
    return ngx_http_output_filter(r, &out);
} /* ngx_http_exert_handler */