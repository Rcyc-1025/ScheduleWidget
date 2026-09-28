/* notify.c — 托盘图标/菜单回调 与 提醒扫描（气泡通知） */
#include "app.h"
#include "notify.h"
#include "widget.h"
#include "storage.h"
#include "lang.h"
#include <stdio.h>
#include <string.h>

static NOTIFYICONDATAW s_nid;
static UINT s_taskbarMsg = 0;
static int  s_added = 0;

void tray_init(void)
{
    s_taskbarMsg = RegisterWindowMessageW(L"TaskbarCreated");
    memset(&s_nid, 0, sizeof(s_nid));
    s_nid.cbSize = sizeof(s_nid);
    s_nid.hWnd = g.hwndMain;
    s_nid.uID = 1;
    s_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    s_nid.uCallbackMessage = WM_APP_TRAY;
    s_nid.hIcon = g.hIcon;
    wcsncpy(s_nid.szTip, tr(S_APP_NAME), 63);
    s_nid.szTip[63] = 0;
    tray_add();
}

/* 语言切换后同步托盘悬停文本 */
void tray_update_tip(void)
{
    if (!s_added) return;
    wcsncpy(s_nid.szTip, tr(S_APP_NAME), 63);
    s_nid.szTip[63] = 0;
    NOTIFYICONDATAW nid = s_nid;
    nid.uFlags = NIF_TIP;
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

void tray_add(void)
{
    if (!g.hwndMain) return;
    s_nid.hWnd = g.hwndMain;
    if (Shell_NotifyIconW(NIM_ADD, &s_nid))
        s_added = 1;
}

void tray_remove(void)
{
    if (s_added) {
        Shell_NotifyIconW(NIM_DELETE, &s_nid);
        s_added = 0;
    }
}

UINT tray_taskbar_msg(void) { return s_taskbarMsg; }

void tray_on_msg(LPARAM lp)
{
    switch ((UINT)lp) {
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
        widget_toggle_visible();
        break;
    case WM_RBUTTONUP:
    case WM_CONTEXTMENU:
        widget_open_menu();
        break;
    default:
        break;
    }
}

/* ================= 提醒扫描 ================= */

void notify_scan(void)
{
    t64 now = tnow();
    Occ occs[256];
    /* 全天日程最早可提前 1 周提醒，窗口需覆盖未来 8 天 */
    int n = schedule_occurrences(now - 86400, now + 8 * 86400, occs, 256);
    for (int i = 0; i < n; i++) {
        Event *ev = schedule_at(occs[i].evIdx);
        if (!ev || ev->remindMinutes < 0) continue;
        if (ev->lastNotified >= occs[i].s) continue;
        /* 定时事件准点 = 起始时刻；全天事件准点 = 当天 9:00 */
        t64 fireAt = occs[i].s - (t64)ev->remindMinutes * 60;
        if (ev->allDay) fireAt += 9 * 3600;
        if (now < fireAt) continue;
        if (now > fireAt + 120) continue;

        wchar_t tm[16], when[64], body[300];
        if (ev->allDay)
            wcsncpy(tm, tr(S_ALLDAY), 15);
        else
            fmt_time(occs[i].s, tm, 16);
        tm[15] = 0;
        t64 d = fireAt - now;
        if (d <= 0) {
            wcsncpy(when, tr(ev->allDay ? S_TODAY : S_NOW), 63);
            when[63] = 0;
        }
        else if (d < 3600)    fmt_rel_mins((d + 59) / 60, when, 64);
        else if (d < 86400)   fmt_rel_hours((d + 3599) / 3600, when, 64);
        else                  fmt_rel_days((d + 86399) / 86400, when, 64);
        _snwprintf(body, 300, L"%s %s\n%s",
                   tm, ev->title[0] ? ev->title : tr(S_UNTITLED), when);
        body[299] = 0;

        NOTIFYICONDATAW nid = s_nid;
        nid.uFlags = NIF_INFO;
        nid.dwInfoFlags = NIIF_INFO;
        wcsncpy(nid.szInfoTitle, tr(S_NOTIFY_TITLE), 63);
        nid.szInfoTitle[63] = 0;
        wcsncpy(nid.szInfo, body, 255);
        nid.szInfo[255] = 0;
        Shell_NotifyIconW(NIM_MODIFY, &nid);

        ev->lastNotified = occs[i].s;
        events_save();
    }
}
