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
    double n;
    lept_type type;
} lept_value;

/* JSON 解析 */
int lept_parse(lept_value *v, const char *json);

lept_type lept_get_type(const lept_value *v);

double lept_get_number(const lept_value *v);

#endif //LEPTJSON_LEPTJSON_H
