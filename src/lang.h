/* lang.h — 界面多语言（中文 / English US / English UK）。
 *
 * 语言配置存 settings.json 的 "lang" 字段：-1 跟随系统 / 0 中文 /
 * 1 English(US) / 2 English(UK)。lang_set() 由 storage 加载设置后调用；
 * lang_current() 返回解析后的语言（AUTO 已按系统 UI 语言展开）。
 *
 * 英美差异集中在格式化辅助函数，而非字符串表：
 *   日期    en-US 9/27/2026   en-GB 27/9/2026   中文 ISO/中文习惯
 *   时间    en-US 12 小时制   其余 24 小时制
 *   周起始  en-US 周日开头    其余周一开头 */
#ifndef LANG_H
#define LANG_H

#include "util.h"   /* t64 */

enum { LANG_AUTO = -1, LANG_ZH = 0, LANG_EN_US = 1, LANG_EN_GB = 2 };

void lang_set(int lang);          /* 接受 LANG_AUTO 或具体语言 */
int  lang_current(void);          /* 解析后语言（只会是 ZH/EN_US/EN_GB） */
int  lang_sunday_first(void);     /* 月历周日开头（en-US） */
int  lang_12h(void);              /* 12 小时制（en-US） */

/* ---- 字符串表 ---- */
typedef enum {
    S_APP_NAME = 0,
    S_UNTITLED, S_NOTITLE, S_TODAY, S_TOMORROW, S_NOW,
    S_ONGOING_UNTIL, S_NO_UPCOMING, S_PILL_HINT, S_NO_EVENTS_DAY,
    S_MENU_NEW, S_MENU_SETTINGS, S_MENU_IMPORT, S_MENU_EXPORT,
    S_MENU_RESTORE, S_MENU_AUTOSTART, S_MENU_CLEAR, S_MENU_EXIT,
    S_CONFIRM_CLEAR, S_EXPORT_OK, S_EXPORT_FAIL,
    S_IMPORT_FAIL, S_IMPORTED_FMT, S_JSON_FILTER, S_ALL_FILES,
    /* 编辑器 */
    S_REM0, S_REM1, S_REM2, S_REM3, S_REM4, S_REM5, S_REM6,
    S_ADREM1, S_ADREM3, S_ADREM4,
    S_UNIT0, S_UNIT1, S_UNIT2, S_UNIT3, S_UNIT4, S_UNIT5,
    S_ERR_TITLE, S_ERR_SDATE, S_ERR_EDATE, S_ERR_STIME, S_ERR_ETIME,
    S_ERR_RANGE_D, S_ERR_RANGE_T, S_CONFIRM_DEL,
    S_ED_NEW, S_ED_EDIT,
    S_LB_TITLE, S_LB_START, S_LB_END, S_LB_REC, S_LB_REMIND,
    S_LB_COLOR, S_LB_NOTE,
    S_ALLDAY, S_EMODE_END, S_EMODE_DUR,
    S_REC_NONE, S_REC_D, S_REC_W, S_REC_M, S_REC_Y, S_REC_C,
    S_UNTIL_NEVER, S_UNTIL_DATE, S_BTN_DEL, S_BTN_SAVE,
    /* 设置 */
    S_SET_TITLE, S_SET_AUTO, S_SET_REMIND, S_SET_LANG, S_LANG_AUTO,
    /* 导入窗口 */
    S_IMP_FILTER, S_IMP_PICKTITLE, S_IMP_ONLY, S_READ_FAIL,
    S_IMP_LOADED, S_PICK_FIRST, S_IMP_ASK, S_IMPORT, S_PARSING,
    S_IMP_UNRECOG, S_MODE_OW, S_MODE_AP,
    S_IMP_OK_SKIP, S_IMP_OK, S_IMP_NONE,
    S_BTN_PICK, S_BTN_SAMPLE, S_IMP_HINT, S_IMP_STATUS0,
    S_SAMPLE_OK, S_SAMPLE_FAIL, S_50LINES,
    /* 提醒 */
    S_NOTIFY_TITLE,
    /* 导入解析错误 */
    S_ERR_NOFILE, S_ERR_NOEXT, S_ERR_JSON, S_ERR_NOROWS,
    /* 文本提取错误 */
    S_TE_XLSX_SHEET, S_TE_XLSX_EMPTY, S_TE_DOCX_MAIN, S_TE_DOCX_EMPTY,
    S_TE_NOOOXML, S_TE_OPEN, S_TE_EMPTY, S_TE_BIG, S_TE_NOMEM, S_TE_OLD,
    S_TE_ENC, S_TE_EMPTY2,
    S_TODAY_SHORT,
    S_COUNT
} StrId;

const wchar_t *tr(int id);        /* 当前语言字符串 */

/* ---- 本地化格式化辅助 ---- */
void fmt_hm(int hh, int mm, wchar_t *out, int cap);          /* 时刻 */
void fmt_md(int m, int d, wchar_t *out, int cap);            /* 月-日 */
void fmt_ymd(int y, int m, int d, wchar_t *out, int cap);    /* 完整日期 */
void fmt_month_title(int y, int m, wchar_t *out, int cap);   /* 月历标题 */
void fmt_day_head(t64 day, t64 today, wchar_t *out, int cap);/* 议程组头 */
void fmt_rel_mins(long long n, wchar_t *out, int cap);       /* N 分钟后 */
void fmt_rel_hours(long long n, wchar_t *out, int cap);
void fmt_rel_days(long long n, wchar_t *out, int cap);
void fmt_every(int n, int unit, wchar_t *out, int cap);      /* 每 N 单位 */
void lang_wday_col(int col, wchar_t *out, int cap);          /* 月历星期列名（按周起始） */
int  lang_weekday0(t64 t);  /* 本周第几天（0 起，周首日依语言：en-US 周日，其余周一） */

/* 全天事件的「全天 · 范围」等组合在 util.c fmt_range 内使用本组函数 */

#endif
