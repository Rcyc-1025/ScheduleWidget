/* settings.c — 设置对话框：开机自启、默认提醒、语言 */
#include "app.h"
#include "settings.h"
#include "editor.h"
#include "storage.h"
#include "widget.h"
#include "theme.h"
#include "lang.h"
#include "notify.h"
#include "ai_window.h"
#include <windowsx.h>

enum {
    IDS_CHK_AUTO = 101, IDS_REMIND = 102, IDS_SAVE = 103, IDS_LANG = 104
};

static const int REMIND_V[7] = { -1, 0, 5, 15, 30, 60, 1440 };

static HWND s_hSet;
static HWND s_hChkAuto, s_hRemind, s_hLang;
static HBRUSH s_setDlgBrush = NULL;

static int remind_index(int minutes)
{
    for (int i = 0; i < 7; i++)
        if (REMIND_V[i] == minutes) return i;
    if (minutes < 0) return 0;
    if (minutes == 0) return 1;
    if (minutes <= 5) return 2;
    if (minutes <= 15) return 3;
    if (minutes <= 30) return 4;
    if (minutes <= 60) return 5;
    return 6;
}

/* 下拉索引 <-> lang 值：0=跟随系统(-1) 1=中文(0) 2=en-US(1) 3=en-GB(2) */
static int lang_to_index(int lang) { return lang + 1; }
static int index_to_lang(int idx) { return idx - 1; }

static void on_save(void)
{
    g.st.autostart = dlg_check_get(s_hChkAuto);
    int ri = ComboBox_GetCurSel(s_hRemind);
    if (ri >= 0 && ri < 7) g.st.defRemind = REMIND_V[ri];
    int li = ComboBox_GetCurSel(s_hLang);
    int newLang = (li >= 0) ? index_to_lang(li) : g.st.lang;
    int langChanged = (newLang != g.st.lang);
    g.st.lang = newLang;
    autostart_set(g.st.autostart);
    settings_save();
    if (langChanged) {
        lang_set(g.st.lang);
        tray_update_tip();
        if (g.hwndMain)
            SetWindowTextW(g.hwndMain, tr(S_APP_NAME));
        ai_window_destroy();   /* 静态文本不刷新，销毁后下次重建 */
    }
    widget_refresh();
    DestroyWindow(s_hSet);
}

static LRESULT CALLBACK set_proc(HWND dlg, UINT msg, WPARAM wp, LPARAM lp,
                                 UINT_PTR id, DWORD_PTR ref)
{
    (void)id; (void)ref;
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDS_SAVE:
            if (HIWORD(wp) == BN_CLICKED) { on_save(); return 0; }
            break;
        case IDCANCEL:
            DestroyWindow(dlg);
            return 0;
        }
        break;
    case WM_CTLCOLORDLG:
        return (LRESULT)s_setDlgBrush;
    case WM_CTLCOLORSTATIC:
        SetBkMode((HDC)wp, TRANSPARENT);
        SetTextColor((HDC)wp, RGB((COL_TXT >> 16) & 0xFF,
                                  (COL_TXT >> 8) & 0xFF, COL_TXT & 0xFF));
        return (LRESULT)s_setDlgBrush;
    case WM_CTLCOLORBTN:   /* 复选框：透明背景，文字随主题 */
        SetBkMode((HDC)wp, TRANSPARENT);
        SetTextColor((HDC)wp, RGB((COL_TXT >> 16) & 0xFF,
                                  (COL_TXT >> 8) & 0xFF, COL_TXT & 0xFF));
        return (LRESULT)s_setDlgBrush;
    case WM_DESTROY:
        s_hSet = NULL;
        if (s_setDlgBrush) { DeleteObject(s_setDlgBrush); s_setDlgBrush = NULL; }
        RemoveWindowSubclass(dlg, set_proc, 1);
        break;
    }
    return DefSubclassProc(dlg, msg, wp, lp);
}

void settings_open(HWND owner)
{
    if (s_hSet) {
        SetForegroundWindow(s_hSet);
        return;
    }
    /* 高度含标题栏（约 30 基准 px），控件自客户区起排，故补偿标题栏高度 */
    HWND dlg = dlg_base_create(owner, tr(S_SET_TITLE), 400, 240);
    if (!dlg) return;
    s_hSet = dlg;
    SetWindowSubclass(dlg, set_proc, 1, 0);
    theme_reload(0);
    theme_apply_dark_frame(dlg);
    if (s_setDlgBrush) DeleteObject(s_setDlgBrush);
    s_setDlgBrush = CreateSolidBrush(RGB((COL_CARD >> 16) & 0xFF,
                                         (COL_CARD >> 8) & 0xFF, COL_CARD & 0xFF));

    int y = 26;
    s_hChkAuto = dlg_ctrl(dlg, L"BUTTON", tr(S_SET_AUTO),
                          WS_TABSTOP | BS_AUTOCHECKBOX, 0, IDS_CHK_AUTO, 24, y, 200, 24);
    dlg_check_set(s_hChkAuto, g.st.autostart);

    y += 44;
    dlg_ctrl(dlg, L"STATIC", tr(S_SET_REMIND), 0, 0, 0, 24, y + 4, 64, 20);
    s_hRemind = dlg_ctrl(dlg, L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST,
                         0, IDS_REMIND, 96, y, 200, 200);
    for (int i = 0; i < 7; i++) ComboBox_AddString(s_hRemind, tr(S_REM0 + i));
    ComboBox_SetCurSel(s_hRemind, remind_index(g.st.defRemind));

    y += 48;
    dlg_ctrl(dlg, L"STATIC", tr(S_SET_LANG), 0, 0, 0, 24, y + 4, 64, 20);
    s_hLang = dlg_ctrl(dlg, L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST,
                       0, IDS_LANG, 96, y, 200, 200);
    ComboBox_AddString(s_hLang, tr(S_LANG_AUTO));
    ComboBox_AddString(s_hLang, L"中文");
    ComboBox_AddString(s_hLang, L"English (US)");
    ComboBox_AddString(s_hLang, L"English (UK)");
    ComboBox_SetCurSel(s_hLang, lang_to_index(g.st.lang));

    y += 48;
    dlg_ctrl(dlg, L"BUTTON", tr(S_BTN_SAVE), WS_TABSTOP | BS_DEFPUSHBUTTON,
             0, IDS_SAVE, 296, y, 84, 30);

    dlg_run(dlg, owner);
}
