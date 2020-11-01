//
// Created by 2h 0x on 2020/10/31.
//

#include <assert.h> /* assert() */
#include <zconf.h> /* NULL, strtod() */
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <memory.h>
#include "leptjson.h"

#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json++; } while(0)
#define lept_init(v) do { (v)->type = LEPT_NULL; } while(0)

typedef struct {
    lept_value v;
    const char *json;
} lept_context;

lept_type lept_get_type(const lept_value *v) {
    return v->type;
}

double lept_get_number(const lept_value *v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->u.n;
}

/* ws = *(%x20 / %x09 / %x0A / %x0D) */
static void lept_parse_whitespace(lept_context *c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')

static int lept_parse_number(lept_context *c, lept_value *v) {
    char *end;
    const char *p = c->json;
    /* 负号 */
    if (*p == '-') p++;
    /* 整数 */
    if (*p == '0') p++;
    else {
        if(!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;
        while (ISDIGIT(*p)) p++;
    }
    /* 小数 */
    if (*p == '.') {
        p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        while (ISDIGIT(*p)) p++;
    }
    /* 指数 */
    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '+' || *p == '-') p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        while (ISDIGIT(*p)) p++;
    }

    errno = 0;
    v->u.n = strtod(c->json, &end);
    /* 数字过大的处理 */
    if (errno == ERANGE && v->u.n == HUGE_VAL) return LEPT_PARSE_NUMBER_TOO_BIG;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_literal(lept_context *c, lept_value *v, const char *literal, lept_type type) {
    EXPECT(c, literal[0]);
    size_t i ;
    for (i = 0; literal[i+1]; i++)
        if (c->json[i] != literal[i+1])
            return LEPT_PARSE_INVALID_VALUE;
    c->json += i;
    v->type = type;
    return LEPT_PARSE_OK;
}

void lept_free(lept_value *v) {
    assert(v != NULL);
    if (v->type == LEPT_STRING)
        free(v->u.s.s);
    v->type = LEPT_NULL;
}

void lept_set_string(lept_value *v, const char *s, size_t len) {
    /* 非空指针（有具体的字符串）或是零长度的字符串都是合法的 */
    assert(v != NULL && (s != NULL || len == 0 ));
    lept_free(v);
    v->u.s.s = (char*) malloc(len + 1);
    /* 当设置一个值为字符串时，需要把参数中的字符串复制一份 */
    memcpy(v->u.s.s, s, len);
    v->u.s.s[len] = '\0';
    v->u.s.len = len;
    v->type = LEPT_STRING;
}

static int lept_parse_string(lept_context *c, lept_value *v) {

}


/* value = null / false / true */
static int lept_parse_value(lept_context *c, lept_value *v) {
    switch (*c->json) {
        case 'n': return lept_parse_literal(c, v, "null", LEPT_NULL);
        case 't': return lept_parse_literal(c, v, "true", LEPT_TRUE);
        case 'f': return lept_parse_literal(c, v, "false", LEPT_FALSE);
        default: return lept_parse_number(c, v);
        /* 空字符 */
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value *v, const char *json) {
    assert(v != NULL);
    int ret;
    lept_context c;
    c.json = json;
    v->type = LEPT_NULL;
    /* 去掉空格 */
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        /* 解析成功后如果后面不是结束符\0, 说明值不是单个 */
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            return LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

