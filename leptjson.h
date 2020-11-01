//
// Created by 2h 0x on 2020/10/31.
//

#ifndef LEPTJSON_LEPTJSON_H
#define LEPTJSON_LEPTJSON_H

/* 定义数据类型 */
typedef enum {
    LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_OBJECT, LEPT_ARRAY
} lept_type;
/* 解析返回状态值 */
enum {
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR,
    LEPT_PARSE_NUMBER_TOO_BIG
};
/* JSON 值 */
typedef struct {
    union {
        struct { char *s; size_t len; } s; /* string */
        double n; /* number */
    } u;
    lept_type type;
} lept_value;

void lept_free(lept_value *v);
#define lept_set_null(v) lept_free(v)

void lept_set_bool(lept_value *v, int b);
int lept_get_bool(const lept_value *v);

void lept_set_string(lept_value *v, const char *s, size_t len);
char* lept_get_string(const lept_value *v);
size_t lept_get_string_length(const lept_value *v);

void lept_set_number(lept_value *v, double n);
double lept_get_number(const lept_value *v) ;

/* JSON 解析 */
int lept_parse(lept_value *v, const char *json);

lept_type lept_get_type(const lept_value *v);


#endif //LEPTJSON_LEPTJSON_H
