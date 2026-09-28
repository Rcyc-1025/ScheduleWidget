/* lang.c — 界面多语言实现：字符串表 + 本地化格式化。
 * 见 lang.h 顶部说明。英美差异全部落在格式化辅助函数里。 */
#include "app.h"
#include "lang.h"
#include <stdio.h>

static int s_cfgLang = LANG_AUTO;

void lang_set(int lang) { s_cfgLang = lang; }

int lang_current(void)
{
    if (s_cfgLang != LANG_AUTO) return s_cfgLang;
    LANGID id = GetUserDefaultUILanguage();
    if (PRIMARYLANGID(id) == LANG_CHINESE) return LANG_ZH;
    if (id == MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK)) return LANG_EN_GB;
    return LANG_EN_US;
}

int lang_sunday_first(void) { return lang_current() == LANG_EN_US; }
int lang_12h(void)          { return lang_current() == LANG_EN_US; }

/* ================= 字符串表（zh / en；en-US 与 en-GB 文案一致） ================= */

static const wchar_t *TBL[S_COUNT][2] = {
    { L"日程助手", L"Schedule Widget" },                              /* S_APP_NAME */
    { L"未命名日程", L"Untitled event" },                             /* S_UNTITLED */
    { L"(无标题)", L"(No title)" },                                   /* S_NOTITLE */
    { L"今天", L"Today" },                                            /* S_TODAY */
    { L"明天", L"Tomorrow" },                                         /* S_TOMORROW */
    { L"现在开始", L"Starting now" },                                 /* S_NOW */
    { L"进行中 · 至 %s", L"In progress · until %s" },                 /* S_ONGOING_UNTIL */
    { L"无即将日程", L"No upcoming events" },                         /* S_NO_UPCOMING */
    { L"点击展开 · ＋ 新建日程", L"Click to expand · + New event" },  /* S_PILL_HINT */
    { L"当日暂无日程", L"No events on this day" },                    /* S_NO_EVENTS_DAY */
    { L"新建日程", L"New Event" },                                    /* S_MENU_NEW */
    { L"设置", L"Settings" },                                         /* S_MENU_SETTINGS */
    { L"导入日程文件…", L"Import Schedule File…" },                   /* S_MENU_IMPORT */
    { L"导出日程…", L"Export Schedule…" },                            /* S_MENU_EXPORT */
    { L"恢复备份（JSON）…", L"Restore Backup (JSON)…" },              /* S_MENU_RESTORE */
    { L"开机自启", L"Run at Startup" },                               /* S_MENU_AUTOSTART */
    { L"清除所有日程", L"Clear All Events" },                         /* S_MENU_CLEAR */
    { L"退出", L"Exit" },                                             /* S_MENU_EXIT */
    { L"确定清除所有日程吗？此操作不可撤销。",
      L"Clear all events? This cannot be undone." },                  /* S_CONFIRM_CLEAR */
    { L"导出成功", L"Export successful" },                            /* S_EXPORT_OK */
    { L"导出失败，无法写入文件", L"Export failed: cannot write the file" }, /* S_EXPORT_FAIL */
    { L"导入失败：文件无法读取或 JSON 无效",
      L"Import failed: cannot read the file or the JSON is invalid" }, /* S_IMPORT_FAIL */
    { L"已导入 %d 条日程", L"Imported %d events" },                   /* S_IMPORTED_FMT */
    { L"JSON 日程文件 (*.json)", L"JSON Schedule (*.json)" },         /* S_JSON_FILTER */
    { L"所有文件", L"All Files" },                                    /* S_ALL_FILES */
    { L"不提醒", L"None" },                                           /* S_REM0 */
    { L"准时提醒", L"On time" },                                      /* S_REM1 */
    { L"提前 5 分钟", L"5 minutes early" },                           /* S_REM2 */
    { L"提前 15 分钟", L"15 minutes early" },                         /* S_REM3 */
    { L"提前 30 分钟", L"30 minutes early" },                         /* S_REM4 */
    { L"提前 1 小时", L"1 hour early" },                              /* S_REM5 */
    { L"提前 1 天", L"1 day early" },                                 /* S_REM6 */
    { L"当天 9:00 提醒", L"At 9:00 on the day" },                     /* S_ADREM1 */
    { L"提前 3 天", L"3 days early" },                                /* S_ADREM3 */
    { L"提前 1 周", L"1 week early" },                                /* S_ADREM4 */
    { L"分钟", L"minutes" },                                          /* S_UNIT0 */
    { L"小时", L"hours" },                                            /* S_UNIT1 */
    { L"天", L"days" },                                               /* S_UNIT2 */
    { L"周", L"weeks" },                                              /* S_UNIT3 */
    { L"月", L"months" },                                             /* S_UNIT4 */
    { L"年", L"years" },                                              /* S_UNIT5 */
    { L"请输入标题", L"Please enter a title" },                       /* S_ERR_TITLE */
    { L"开始日期无效", L"Invalid start date" },                       /* S_ERR_SDATE */
    { L"结束日期无效", L"Invalid end date" },                         /* S_ERR_EDATE */
    { L"开始时间无效", L"Invalid start time" },                       /* S_ERR_STIME */
    { L"结束时间无效", L"Invalid end time" },                         /* S_ERR_ETIME */
    { L"结束日期不能早于开始日期",
      L"End date cannot be earlier than start date" },                /* S_ERR_RANGE_D */
    { L"结束时间不能早于开始时间",
      L"End time cannot be earlier than start time" },                /* S_ERR_RANGE_T */
    { L"确定删除该日程吗？", L"Delete this event?" },                 /* S_CONFIRM_DEL */
    { L"新建日程", L"New Event" },                                    /* S_ED_NEW */
    { L"编辑日程", L"Edit Event" },                                   /* S_ED_EDIT */
    { L"标题", L"Title" },                                            /* S_LB_TITLE */
    { L"开始", L"Start" },                                            /* S_LB_START */
    { L"结束", L"End" },                                              /* S_LB_END */
    { L"循环", L"Repeat" },                                           /* S_LB_REC */
    { L"提醒", L"Reminder" },                                         /* S_LB_REMIND */
    { L"颜色", L"Color" },                                            /* S_LB_COLOR */
    { L"备注", L"Note" },                                             /* S_LB_NOTE */
    { L"全天", L"All day" },                                          /* S_ALLDAY */
    { L"指定结束时间", L"Set end time" },                             /* S_EMODE_END */
    { L"持续时长", L"Duration" },                                     /* S_EMODE_DUR */
    { L"不重复", L"Never" },                                          /* S_REC_NONE */
    { L"每天", L"Daily" },                                            /* S_REC_D */
    { L"每周", L"Weekly" },                                           /* S_REC_W */
    { L"每月", L"Monthly" },                                          /* S_REC_M */
    { L"每年", L"Yearly" },                                           /* S_REC_Y */
    { L"自定义", L"Custom" },                                         /* S_REC_C */
    { L"永不停止", L"Never ends" },                                   /* S_UNTIL_NEVER */
    { L"指定截止日期", L"End by date" },                              /* S_UNTIL_DATE */
    { L"删除", L"Delete" },                                           /* S_BTN_DEL */
    { L"保存", L"Save" },                                             /* S_BTN_SAVE */
    { L"设置", L"Settings" },                                         /* S_SET_TITLE */
    { L"开机自启", L"Run at startup" },                               /* S_SET_AUTO */
    { L"默认提醒", L"Default reminder" },                             /* S_SET_REMIND */
    { L"语言", L"Language" },                                         /* S_SET_LANG */
    { L"跟随系统", L"System default" },                               /* S_LANG_AUTO */
    { L"日程文件", L"Schedule Files" },                               /* S_IMP_FILTER */
    { L"选择要导入的日程文件", L"Select a schedule file to import" }, /* S_IMP_PICKTITLE */
    { L"仅支持 ics / csv / xlsx / json 文件",
      L"Only ics / csv / xlsx / json files are supported" },          /* S_IMP_ONLY */
    { L"读取文件失败", L"Failed to read the file" },                  /* S_READ_FAIL */
    { L"已加载: %s（点「导入日程」开始）",
      L"Loaded: %s (click \"Import\" to start)" },                    /* S_IMP_LOADED */
    { L"请先选择文件", L"Please select a file first" },               /* S_PICK_FIRST */
    { L"当前已有 %d 条日程。\n\n"
      L"是：覆盖（清空现有日程后导入）\n"
      L"否：追加（保留现有日程并新增）\n"
      L"取消：放弃导入",
      L"You already have %d events.\n\n"
      L"Yes: Overwrite (clear existing events before import)\n"
      L"No: Append (keep existing events and add new ones)\n"
      L"Cancel: Abort the import" },                                  /* S_IMP_ASK */
    { L"导入日程", L"Import Schedule" },                              /* S_IMPORT */
    { L"正在解析…", L"Parsing…" },                                    /* S_PARSING */
    { L"无法识别该文件，请点「示范文件」对照格式",
      L"Cannot recognize this file. Click \"Sample Files\" for format reference." }, /* S_IMP_UNRECOG */
    { L"（已覆盖原有日程）", L" (overwrote existing events)" },       /* S_MODE_OW */
    { L"（追加）", L" (appended)" },                                  /* S_MODE_AP */
    { L"成功导入 %d 条日程（%d 条无效已跳过）%s",
      L"Imported %d events (%d invalid skipped)%s" },                 /* S_IMP_OK_SKIP */
    { L"成功导入 %d 条日程%s", L"Imported %d events%s" },             /* S_IMP_OK */
    { L"未找到有效日程行（%d 行无效）%s",
      L"No valid schedule rows found (%d invalid)%s" },               /* S_IMP_NONE */
    { L"选择文件…", L"Select File…" },                                /* S_BTN_PICK */
    { L"示范文件", L"Sample Files" },                                 /* S_BTN_SAMPLE */
    { L"支持 ics / csv / xlsx / json 四种格式。\r\n"
      L"点右上角「示范文件」可生成四种格式的范例，\r\n"
      L"或直接把文件拖入此窗口。",
      L"Supports ics / csv / xlsx / json.\r\n"
      L"Click \"Sample Files\" (top right) to generate examples in all four formats,\r\n"
      L"or drag a file into this window." },                          /* S_IMP_HINT */
    { L"请先选择 ics / csv / xlsx / json 文件，或直接拖入此窗口",
      L"Select an ics / csv / xlsx / json file, or drag it into this window" }, /* S_IMP_STATUS0 */
    { L"已在数据目录生成 4 个示范文件并打开文件夹，可对照修改后导入",
      L"4 sample files were generated in the data folder and the folder was opened; edit them and import" }, /* S_SAMPLE_OK */
    { L"示范文件生成失败（数据目录不可写），请检查后重试。",
      L"Failed to generate sample files (data folder is not writable). Please check and try again." }, /* S_SAMPLE_FAIL */
    { L"…（仅显示前 50 行）", L"…(first 50 lines only)" },            /* S_50LINES */
    { L"日程提醒", L"Schedule Reminder" },                            /* S_NOTIFY_TITLE */
    { L"未选择文件", L"No file selected" },                           /* S_ERR_NOFILE */
    { L"文件没有扩展名", L"The file has no extension" },              /* S_ERR_NOEXT */
    { L"JSON 结构无法识别（应为日程数组）",
      L"Unrecognized JSON structure (expected an array of events)" }, /* S_ERR_JSON */
    { L"未能识别出有效的日程行", L"No valid schedule rows recognized" }, /* S_ERR_NOROWS */
    { L"xlsx 中未找到工作表（文件可能损坏）",
      L"No worksheet found in the xlsx (the file may be corrupted)" }, /* S_TE_XLSX_SHEET */
    { L"xlsx 工作表内容为空或解析失败",
      L"The xlsx worksheet is empty or parsing failed" },             /* S_TE_XLSX_EMPTY */
    { L"docx 中未找到 word/document.xml（文件可能损坏）",
      L"word/document.xml not found in the docx (the file may be corrupted)" }, /* S_TE_DOCX_MAIN */
    { L"docx 内容为空或解析失败",
      L"The docx content is empty or parsing failed" },               /* S_TE_DOCX_EMPTY */
    { L"压缩包中未找到 xlsx/docx 内容",
      L"No xlsx/docx content found in the archive" },                 /* S_TE_NOOOXML */
    { L"无法打开文件", L"Cannot open the file" },                     /* S_TE_OPEN */
    { L"文件为空或无法获取大小",
      L"The file is empty or its size cannot be determined" },        /* S_TE_EMPTY */
    { L"文件超过 20MB，请精简后再试",
      L"The file exceeds 20 MB; please trim it and try again" },      /* S_TE_BIG */
    { L"内存不足", L"Out of memory" },                                /* S_TE_NOMEM */
    { L"旧格式（.doc/.xls）请先另存为 .docx/.xlsx/CSV 再导入",
      L"Legacy format (.doc/.xls); please save as .docx/.xlsx/CSV first" }, /* S_TE_OLD */
    { L"编码转换失败（请存为 UTF-8 文本）",
      L"Encoding conversion failed (please save as UTF-8 text)" },     /* S_TE_ENC */
    { L"文件内容为空", L"The file content is empty" },                 /* S_TE_EMPTY2 */
    { L"今", L"T" },                                                   /* S_TODAY_SHORT */
};

const wchar_t *tr(int id)
{
    if (id < 0 || id >= S_COUNT) return L"";
    return TBL[id][lang_current() == LANG_ZH ? 0 : 1];
}

/* ================= 名称表 ================= */

static const wchar_t *MON_FULL[12] = {
    L"January", L"February", L"March", L"April", L"May", L"June",
    L"July", L"August", L"September", L"October", L"November", L"December"
};
static const wchar_t *MON_SHORT[12] = {
    L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun",
    L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec"
};
/* 0=周一 … 6=周日 */
static const wchar_t *WDAY_SHORT[7] = {
    L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat", L"Sun"
};

/* ================= 格式化辅助 ================= */

void fmt_hm(int hh, int mm, wchar_t *out, int cap)
{
    if (lang_12h()) {
        int h12 = hh % 12; if (h12 == 0) h12 = 12;
        _snwprintf(out, cap, L"%d:%02d %s", h12, mm, hh < 12 ? L"AM" : L"PM");
    } else {
        _snwprintf(out, cap, L"%02d:%02d", hh, mm);
    }
    out[cap - 1] = 0;
}

void fmt_md(int m, int d, wchar_t *out, int cap)
{
    switch (lang_current()) {
    case LANG_EN_US: _snwprintf(out, cap, L"%d/%d", m, d); break;
    case LANG_EN_GB: _snwprintf(out, cap, L"%d/%d", d, m); break;
    default:         _snwprintf(out, cap, L"%d月%d日", m, d); break;
    }
    out[cap - 1] = 0;
}

void fmt_ymd(int y, int m, int d, wchar_t *out, int cap)
{
    switch (lang_current()) {
    case LANG_EN_US: _snwprintf(out, cap, L"%d/%d/%04d", m, d, y); break;
    case LANG_EN_GB: _snwprintf(out, cap, L"%d/%d/%04d", d, m, y); break;
    default:         _snwprintf(out, cap, L"%04d-%02d-%02d", y, m, d); break;
    }
    out[cap - 1] = 0;
}

void fmt_month_title(int y, int m, wchar_t *out, int cap)
{
    if (lang_current() == LANG_ZH)
        _snwprintf(out, cap, L"%d年%d月", y, m);
    else
        _snwprintf(out, cap, L"%s %d", MON_FULL[(m - 1 + 12) % 12], y);
    out[cap - 1] = 0;
}

/* 议程组头：今天/明天前缀 + 日期 + 星期 */
void fmt_day_head(t64 day, t64 today, wchar_t *out, int cap)
{
    int y1, m1, d1; tbreak(day, &y1, &m1, &d1, NULL, NULL);
    int cy2; tbreak(tnow(), &cy2, NULL, NULL, NULL, NULL);
    int wdi = weekday_mon0(day);
    int isToday = (day == today);
    int isTomorrow = (day == today + 86400);

    if (lang_current() == LANG_ZH) {
        static const wchar_t *WD = L"一二三四五六日";
        wchar_t date[32];
        if (y1 != cy2) _snwprintf(date, 32, L"%d年%d月%d日", y1, m1, d1);
        else           _snwprintf(date, 32, L"%d月%d日", m1, d1);
        date[31] = 0;
        if (isToday)         _snwprintf(out, cap, L"%s %s 周%c", tr(S_TODAY), date, WD[wdi]);
        else if (isTomorrow) _snwprintf(out, cap, L"%s %s 周%c", tr(S_TOMORROW), date, WD[wdi]);
        else                 _snwprintf(out, cap, L"%s 周%c", date, WD[wdi]);
    } else {
        wchar_t date[48];
        if (lang_current() == LANG_EN_US) {
            if (y1 != cy2) _snwprintf(date, 48, L"%s, %s %d, %d", WDAY_SHORT[wdi], MON_SHORT[m1 - 1], d1, y1);
            else           _snwprintf(date, 48, L"%s, %s %d", WDAY_SHORT[wdi], MON_SHORT[m1 - 1], d1);
        } else {
            if (y1 != cy2) _snwprintf(date, 48, L"%s, %d %s %d", WDAY_SHORT[wdi], d1, MON_SHORT[m1 - 1], y1);
            else           _snwprintf(date, 48, L"%s, %d %s", WDAY_SHORT[wdi], d1, MON_SHORT[m1 - 1]);
        }
        date[47] = 0;
        if (isToday)         _snwprintf(out, cap, L"%s, %s", tr(S_TODAY), date);
        else if (isTomorrow) _snwprintf(out, cap, L"%s, %s", tr(S_TOMORROW), date);
        else                 _snwprintf(out, cap, L"%s", date);
    }
    out[cap - 1] = 0;
}

void fmt_rel_mins(long long n, wchar_t *out, int cap)
{
    if (lang_current() == LANG_ZH) _snwprintf(out, cap, L"%lld分钟后", n);
    else if (n == 1) _snwprintf(out, cap, L"in 1 minute");
    else             _snwprintf(out, cap, L"in %lld minutes", n);
    out[cap - 1] = 0;
}

void fmt_rel_hours(long long n, wchar_t *out, int cap)
{
    if (lang_current() == LANG_ZH) _snwprintf(out, cap, L"%lld小时后", n);
    else if (n == 1) _snwprintf(out, cap, L"in 1 hour");
    else             _snwprintf(out, cap, L"in %lld hours", n);
    out[cap - 1] = 0;
}

void fmt_rel_days(long long n, wchar_t *out, int cap)
{
    if (lang_current() == LANG_ZH) _snwprintf(out, cap, L"%lld天后", n);
    else if (n == 1) _snwprintf(out, cap, L"in 1 day");
    else             _snwprintf(out, cap, L"in %lld days", n);
    out[cap - 1] = 0;
}

/* 自定义循环：每 N 分钟/小时/天/周/月/年（unit 为 TimeUnit 0..5） */
void fmt_every(int n, int unit, wchar_t *out, int cap)
{
    if (unit < 0 || unit > 5) unit = 2;
    if (lang_current() == LANG_ZH) {
        _snwprintf(out, cap, L"每%d%s", n, tr(S_UNIT0 + unit));
    } else {
        static const wchar_t *SG[6] = {
            L"minute", L"hour", L"day", L"week", L"month", L"year"
        };
        static const wchar_t *PL[6] = {
            L"minutes", L"hours", L"days", L"weeks", L"months", L"years"
        };
        if (n == 1) _snwprintf(out, cap, L"Every %s", SG[unit]);
        else        _snwprintf(out, cap, L"Every %d %s", n, PL[unit]);
    }
    out[cap - 1] = 0;
}

/* 月历星期列名：列 0 是本周第一天（en-US 周日，其余周一） */
void lang_wday_col(int col, wchar_t *out, int cap)
{
    if (cap <= 0) return;
    if (col < 0 || col > 6) { out[0] = 0; return; }
    int wdi = lang_sunday_first() ? (col + 6) % 7 : col;   /* 0=周一 */
    if (lang_current() == LANG_ZH) {
        static const wchar_t *WD = L"一二三四五六日";
        _snwprintf(out, cap, L"%c", WD[wdi]);
    } else {
        static const wchar_t *ONE[7] = { L"M", L"T", L"W", L"T", L"F", L"S", L"S" };
        _snwprintf(out, cap, L"%s", ONE[wdi]);
    }
    out[cap - 1] = 0;
}

int lang_weekday0(t64 t)
{
    int w = weekday_mon0(t);              /* 0=周一 */
    if (lang_sunday_first()) w = (w + 1) % 7;
    return w;
}
