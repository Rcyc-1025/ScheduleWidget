/* lunar.h -- 农历与二十四节气换算（公历 1900-01-31 ~ 2100-12-31）。
 *
 * 纯 C 实现，不依赖第三方库；年表与节气表为内置历法数据。
 * 超出支持范围时所有查询返回「无」（不显示、不崩溃）。
 */
#ifndef LUNAR_H
#define LUNAR_H

#include <stddef.h>   /* wchar_t：C 模式下由 CRT 头提供，不能靠编译器内置 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LunarDate {
    int year;   /* 农历年 1900..2100 */
    int month;  /* 农历月 1..12 */
    int day;    /* 农历日 1..30 */
    int leap;   /* 非 0 表示闰月 */
} LunarDate;

/* 公历转农历：成功返回 1 并写入 out，超出支持范围返回 0。 */
int lunar_from_solar(int y, int m, int d, LunarDate* out);

/* 该公历日是否为节气日：返回节气序号（0=小寒 .. 23=冬至），不是则 -1。 */
int lunar_solar_term(int y, int m, int d);

/* 节气名称；序号越界返回空串。 */
const wchar_t* lunar_term_name(int term);

/* 日历格子里显示的农历短文本（节日 > 节气 > 农历日；初一显示月份名）。
 * 写入 buf 并返回 buf；buf 容量不足时截断；不支持该日期时 buf 为空串。 */
const wchar_t* lunar_day_label(int y, int m, int d, wchar_t* outBuf, int bufCap);

#ifdef __cplusplus
}
#endif

#endif /* LUNAR_H */
