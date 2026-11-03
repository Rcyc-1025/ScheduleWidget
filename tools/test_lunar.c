/* test_lunar.c — 农历换算 / 传统节日 / 二十四节气自测。
 *
 * 覆盖三部分：
 *   1) 41 个权威日期（多年春节、中秋、端午、闰月初一、除夕、1900 与 2100 边界）；
 *   2) 日历格子里的农历显示文本（节日 > 节气 > 农历日，初一显示月份名，闰月加「闰」）；
 *   3) 全区间不变量：1900-01-31 ~ 2100-12-31 每天都可换算、字段合法、文本非空，
 *      每年 24 个节气、每月各 2 个，春节/除夕标注正确，越界日期不显示。
 *
 * 运行：tools\build_test.bat test_lunar
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "../src/lunar.c"

static int g_fail = 0;

#define CHECK(cond, msg) \
    do { if (cond) printf("PASS: %s\n", msg); \
         else { printf("FAIL: %s\n", msg); g_fail = 1; } } while (0)

/* 公历 -> 农历的权威日期（农历年月日、闰月标记来自历法数据） */
static const struct {
    int y, m, d;      /* 公历 */
    int ly, lm, ld;   /* 农历年/月/日 */
    int leap;         /* 闰月 */
} CASE[] = {
    { 1900, 1, 31, 1900, 1, 1, 0 },  /* 春节 1900 */
    { 1912, 2, 18, 1912, 1, 1, 0 },  /* 春节 1912 */
    { 1949, 1, 29, 1949, 1, 1, 0 },  /* 春节 1949 */
    { 1984, 2, 2, 1984, 1, 1, 0 },  /* 春节 1984 */
    { 2000, 2, 5, 2000, 1, 1, 0 },  /* 春节 2000 */
    { 2008, 2, 7, 2008, 1, 1, 0 },  /* 春节 2008 */
    { 2016, 2, 8, 2016, 1, 1, 0 },  /* 春节 2016 */
    { 2020, 1, 25, 2020, 1, 1, 0 },  /* 春节 2020 */
    { 2023, 1, 22, 2023, 1, 1, 0 },  /* 春节 2023 */
    { 2024, 2, 10, 2024, 1, 1, 0 },  /* 春节 2024 */
    { 2025, 1, 29, 2025, 1, 1, 0 },  /* 春节 2025 */
    { 2026, 2, 17, 2026, 1, 1, 0 },  /* 春节 2026 */
    { 2033, 1, 31, 2033, 1, 1, 0 },  /* 春节 2033 */
    { 2044, 1, 30, 2044, 1, 1, 0 },  /* 春节 2044 */
    { 2099, 1, 21, 2099, 1, 1, 0 },  /* 春节 2099 */
    { 2100, 2, 9, 2100, 1, 1, 0 },  /* 春节 2100 */
    { 2004, 3, 21, 2004, 2, 1, 1 },  /* 闰二月 2004 */
    { 2006, 8, 24, 2006, 7, 1, 1 },  /* 闰七月 2006 */
    { 2009, 6, 23, 2009, 5, 1, 1 },  /* 闰五月 2009 */
    { 2012, 5, 21, 2012, 4, 1, 1 },  /* 闰四月 2012 */
    { 2017, 7, 23, 2017, 6, 1, 1 },  /* 闰六月 2017 */
    { 2020, 5, 23, 2020, 4, 1, 1 },  /* 闰四月 2020 */
    { 2023, 3, 22, 2023, 2, 1, 1 },  /* 闰二月 2023 */
    { 2025, 7, 25, 2025, 6, 1, 1 },  /* 闰六月 2025 */
    { 2033, 12, 22, 2033, 11, 1, 1 },  /* 闰冬月 2033 */
    { 2023, 9, 29, 2023, 8, 15, 0 },  /* 中秋 2023 */
    { 2024, 9, 17, 2024, 8, 15, 0 },  /* 中秋 2024 */
    { 2025, 10, 6, 2025, 8, 15, 0 },  /* 中秋 2025 */
    { 2026, 9, 25, 2026, 8, 15, 0 },  /* 中秋 2026 */
    { 2030, 9, 12, 2030, 8, 15, 0 },  /* 中秋 2030 */
    { 2099, 9, 29, 2099, 8, 15, 0 },  /* 中秋 2099 */
    { 2023, 6, 22, 2023, 5, 5, 0 },  /* 端午 2023 */
    { 2024, 6, 10, 2024, 5, 5, 0 },  /* 端午 2024 */
    { 2025, 5, 31, 2025, 5, 5, 0 },  /* 端午 2025 */
    { 2026, 6, 19, 2026, 5, 5, 0 },  /* 端午 2026 */
    { 2030, 6, 5, 2030, 5, 5, 0 },  /* 端午 2030 */
    { 2024, 2, 9, 2023, 12, 30, 0 },  /* 除夕 2023 */
    { 2025, 1, 28, 2024, 12, 29, 0 },  /* 除夕 2024 */
    { 2026, 2, 16, 2025, 12, 29, 0 },  /* 除夕 2025 */
    { 2027, 2, 5, 2026, 12, 29, 0 },  /* 除夕 2026 */
    { 2100, 12, 31, 2100, 12, 1, 0 },  /* 支持范围末日 */
};

/* 日历格子里应显示的文本 */
static const struct {
    int y, m, d;
    const wchar_t *label;
} LABEL[] = {
    { 2026, 2, 17, L"春节" },
    { 2026, 3, 3, L"元宵节" },
    { 2026, 4, 5, L"清明" },
    { 2026, 6, 19, L"端午节" },
    { 2026, 8, 19, L"七夕" },
    { 2026, 9, 25, L"中秋节" },
    { 2026, 10, 18, L"重阳节" },
    { 2027, 1, 15, L"腊八节" },
    { 2026, 2, 16, L"除夕" },
    { 2025, 7, 25, L"闰六月" },
    { 2033, 12, 22, L"闰冬月" },
    { 2026, 2, 18, L"雨水" },
    { 2026, 3, 20, L"龙抬头" },
    { 2026, 5, 5, L"立夏" },
    { 2024, 4, 4, L"清明" },
    { 2026, 12, 8, L"三十" },
    { 2026, 1, 1, L"十三" },
    { 1900, 1, 31, L"春节" },
    { 2100, 12, 31, L"腊月" },
};

static int month_days_g(int y, int m)
{
    static const int days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (m == 2 && (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0))) return 29;
    return days[m - 1];
}

/* 下一公历日（就地推进，用于全区间扫描） */
static void next_day(int *y, int *m, int *d)
{
    (*d)++;
    if (*d > month_days_g(*y, *m)) {
        *d = 1;
        (*m)++;
        if (*m > 12) { *m = 1; (*y)++; }
    }
}

/* 节日名（不含除夕，除夕按腊月最后一天单算） */
static int is_festival(int lm, int ld, int leap)
{
    static const struct { int m, d; } F[] = {
        { 1, 1 }, { 1, 15 }, { 2, 2 }, { 5, 5 }, { 7, 7 },
        { 7, 15 }, { 8, 15 }, { 9, 9 }, { 12, 8 }
    };
    int i;
    if (leap) return 0;
    for (i = 0; i < (int)(sizeof(F) / sizeof(F[0])); i++)
        if (F[i].m == lm && F[i].d == ld) return 1;
    return 0;
}

int main(void)
{
    int i, n;
    char msg[160];

    /* ---- 1. 权威日期 ---- */
    n = (int)(sizeof(CASE) / sizeof(CASE[0]));
    for (i = 0; i < n; i++) {
        LunarDate ld;
        int ok = lunar_from_solar(CASE[i].y, CASE[i].m, CASE[i].d, &ld);
        ok = ok && ld.year == CASE[i].ly && ld.month == CASE[i].lm &&
             ld.day == CASE[i].ld && (ld.leap ? 1 : 0) == CASE[i].leap;
        sprintf(msg, "lunar %04d-%02d-%02d = %d-%d-%d%s",
                CASE[i].y, CASE[i].m, CASE[i].d, CASE[i].ly, CASE[i].lm,
                CASE[i].ld, CASE[i].leap ? " leap" : "");
        CHECK(ok, msg);
    }
    printf("INFO: %d authoritative dates checked\n", n);

    /* ---- 2. 显示文本 ---- */
    n = (int)(sizeof(LABEL) / sizeof(LABEL[0]));
    for (i = 0; i < n; i++) {
        wchar_t buf[32];
        const wchar_t *got = lunar_day_label(LABEL[i].y, LABEL[i].m, LABEL[i].d,
                                             buf, 32);
        int ok = (wcscmp(got, LABEL[i].label) == 0);
        sprintf(msg, "label %04d-%02d-%02d", LABEL[i].y, LABEL[i].m, LABEL[i].d);
        CHECK(ok, msg);
        if (!ok) printf("  got len=%d want len=%d\n", (int)wcslen(got),
                        (int)wcslen(LABEL[i].label));
    }

    /* ---- 3. 全区间不变量 ---- */
    {
        int y = 1900, m = 1, d = 31;
        int days = 0, badConv = 0, badField = 0, badLabel = 0;
        int badStep = 0, badCny = 0, badChuxi = 0, badTerm = 0;
        LunarDate prev = { 0, 0, 0, 0 };
        int havePrev = 0;

        for (;;) {
            LunarDate ld;
            wchar_t buf[32];
            const wchar_t *lab;
            if (!lunar_from_solar(y, m, d, &ld)) { badConv++; }
            else {
                if (ld.year < 1900 || ld.year > 2100 || ld.month < 1 || ld.month > 12 ||
                    ld.day < 1 || ld.day > 30 || (ld.leap != 0 && ld.leap != 1))
                    badField++;
                lab = lunar_day_label(y, m, d, buf, 32);
                if (!lab[0] || wcslen(lab) > 8) badLabel++;
                if (ld.month == 1 && ld.day == 1 && !ld.leap && wcscmp(lab, L"春节") != 0)
                    badCny++;
                /* 节气当天显示节气名（被节日或除夕顶替时除外） */
                {
                    int t = lunar_solar_term(y, m, d);
                    if (t >= 0 && wcscmp(lab, lunar_term_name(t)) != 0 &&
                        !is_festival(ld.month, ld.day, ld.leap) &&
                        wcscmp(lab, L"除夕") != 0)
                        badTerm++;
                }
                if (havePrev) {
                    int same = (ld.year == prev.year && ld.month == prev.month &&
                                ld.leap == prev.leap);
                    int okStep = (same && ld.day == prev.day + 1) || (ld.day == 1);
                    if (ld.day == 1 && !same) {
                        /* 跨月：正常顺延（同月闰月跳转允许 month 不变、leap 切换） */
                        if (!(ld.month == prev.month + 1 ||
                              (ld.leap && ld.month == prev.month) ||
                              (ld.year == prev.year + 1 && ld.month == 1 && !ld.leap)))
                            okStep = 0;
                    }
                    if (!okStep) badStep++;
                }
                /* 除夕：腊月最后一天（次日为正月初一） */
                if (ld.month == 12 && !ld.leap) {
                    int ny = y, nm = m, nd = d;
                    LunarDate nl;
                    next_day(&ny, &nm, &nd);
                    if (lunar_from_solar(ny, nm, nd, &nl) &&
                        nl.month == 1 && nl.day == 1 && !nl.leap &&
                        wcscmp(lab, L"除夕") != 0)
                        badChuxi++;
                }
                prev = ld;
                havePrev = 1;
            }
            days++;
            if (y == 2100 && m == 12 && d == 31) break;
            next_day(&y, &m, &d);
        }
        printf("INFO: scanned %d days, conv errors %d, field errors %d, "
               "label errors %d, step errors %d, CNY errors %d, "
               "Chuxi errors %d, term errors %d\n",
               days, badConv, badField, badLabel, badStep, badCny, badChuxi, badTerm);
        CHECK(days == 73384, "full range day count");
        CHECK(badConv == 0, "every day converts");
        CHECK(badField == 0, "lunar fields in range");
        CHECK(badLabel == 0, "label non-empty and short");
        CHECK(badStep == 0, "lunar dates advance one by one");
        CHECK(badCny == 0, "lunar new year shows Chunjie");
        CHECK(badChuxi == 0, "Chuxi labelled on last day of 12th month");
        CHECK(badTerm == 0, "solar term days show the term name");
    }

    /* ---- 4. 节气：每年 24 个、每月 2 个；名称与序号对应 ---- */
    {
        int y, m, d;
        int badCount = 0, badName = 0;
        for (y = 1900; y <= 2100; y++) {
            int perMonth[13] = { 0 };
            int total = 0;
            for (m = 1; m <= 12; m++) {
                for (d = 1; d <= month_days_g(y, m); d++) {
                    int t = lunar_solar_term(y, m, d);
                    if (t >= 0) {
                        total++;
                        perMonth[m]++;
                        if (t != (m - 1) * 2 && t != (m - 1) * 2 + 1) badName++;
                    }
                }
            }
            if (total != 24) badCount++;
            for (m = 1; m <= 12; m++)
                if (perMonth[m] != 2) badCount++;
        }
        CHECK(badCount == 0, "24 solar terms per year, 2 per month");
        CHECK(badName == 0, "term index matches its month");
    }

    /* ---- 5. 具体节气日期与名称 ---- */
    {
        CHECK(lunar_solar_term(2026, 4, 5) == 6, "2026-04-05 is Qingming");
        CHECK(lunar_solar_term(2026, 10, 23) == 19, "2026-10-23 is Shuangjiang");
        CHECK(lunar_solar_term(2033, 2, 3) == 2, "2033-02-03 is Lichun");
        CHECK(lunar_solar_term(2033, 2, 4) == -1, "2033-02-04 is not a term day");
        CHECK(lunar_solar_term(2026, 4, 6) == -1, "2026-04-06 is not a term day");
        CHECK(wcscmp(lunar_term_name(6), L"清明") == 0, "term name 6 = Qingming");
        CHECK(wcscmp(lunar_term_name(-1), L"") == 0, "term name -1 empty");
        CHECK(wcscmp(lunar_term_name(24), L"") == 0, "term name 24 empty");
    }

    /* ---- 6. 越界：不显示、不崩溃 ---- */
    {
        LunarDate ld;
        wchar_t buf[32];
        memset(&ld, 0x7f, sizeof(ld));
        CHECK(lunar_from_solar(1900, 1, 30, &ld) == 0, "1900-01-30 unsupported");
        CHECK(lunar_from_solar(1899, 12, 31, &ld) == 0, "1899-12-31 unsupported");
        CHECK(lunar_from_solar(2101, 1, 1, &ld) == 0, "2101-01-01 unsupported");
        CHECK(lunar_from_solar(2026, 2, 30, &ld) == 0, "2026-02-30 rejected");
        CHECK(lunar_from_solar(2026, 13, 1, &ld) == 0, "month 13 rejected");
        CHECK(lunar_from_solar(2050, 1, 1, NULL) == 0, "NULL out rejected");
        CHECK(lunar_solar_term(1899, 6, 1) == -1, "term out of range");
        buf[0] = L'x';
        CHECK(lunar_day_label(2101, 1, 1, buf, 32)[0] == 0, "out-of-range label empty");
        CHECK(lunar_day_label(2026, 2, 17, NULL, 32)[0] == 0, "NULL buffer tolerated");
        /* 截断安全：cap=2 时只留 1 个字 + 结尾 0 */
        CHECK(wcslen(lunar_day_label(2025, 7, 25, buf, 2)) == 1,
              "label truncated to cap-1");
        CHECK(wcslen(lunar_day_label(2025, 7, 25, buf, 1)) == 0, "cap=1 yields empty");
    }

    printf(g_fail ? "RESULT: FAILED\n" : "RESULT: ALL PASS\n");
    return g_fail ? 1 : 0;
}
