//
// Created by 2h 0x on 2020/10/31.
//

#include <assert.h> /* assert() */
#include <zconf.h> /* NULL, strtod() */
#include <stdlib.h>
#include "leptjson.h"

#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    lept_value v;
    const char *json;
} lept_context;

lept_type lept_get_type(const lept_value *v) {
    return v->type;
}

double lept_get_number(const lept_value *v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}

/* ws = *(%x20 / %x09 / %x0A / %x0D) */
static void lept_parse_whitespace(lept_context *c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

/* null = "null" */
static int lept_parse_null(lept_context *c, lept_value *v) {
    EXPECT(c, 'n');
    while (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}

/*  */
static int lept_parse_true(lept_context *c, lept_value *v) {
    EXPECT(c, 't');
    while (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

/*  */
static int lept_parse_false(lept_context *c, lept_value *v) {
    EXPECT(c, 'f');
    while (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context *c, lept_value *v) {
    char *end;
    v->n = strtod(c->json, &end);
    /* \TODO  validate number */
    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_literal(lept_context *c, lept_value *v, const char flag) {

    return LEPT_PARSE_OK;
}

/* value = null / false / true */
static int lept_parse_value(lept_context *c, lept_value *v) {
    switch (*c->json) {
        case 'n':
            return lept_parse_null(c, v);
        case 't':
            return lept_parse_true(c, v);
        case 'f':
            return lept_parse_false(c, v);
            /* 空字符 */
        case '\0':
            return LEPT_PARSE_EXPECT_VALUE;
        default:
            return lept_parse_number(c, v);
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

