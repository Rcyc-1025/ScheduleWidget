# 日程助手 ScheduleWidget

[中文](#中文) | [English](#english)

---

## 中文

一个轻量的 Windows 桌面日程小组件，纯 C + Win32 API + GDI+ 编写，无任何运行时依赖（静态链接 `/MT`，单个 exe 即可运行）。常驻桌面右下角，提供月历、当日议程、定时提醒与日程文件导入。

### 解决什么问题

- **想看日程却不想开 App**：日历/待办软件启动慢、弹窗多；本组件直接钉在桌面层，随时瞟一眼就能看到月历与当天安排，收起后只是一枚小胶囊。
- **已有日程分散在各处**：日历（.ics）、Excel/表格（.xlsx/.csv）、自建脚本（.json）里的日程可以一次性导入，不必手动重录。
- **怕错过、也怕被打扰**：可按日程设置提前提醒；不想要常驻窗口时可折叠成胶囊自动轮播。
- **讨厌安装依赖**：单 exe、无 .NET/无运行库、数据全在本地，拷走即迁移。

### 功能特性

- **月历视图**：标记有日程的日期，高亮今天与选中日，左右翻月，「今」一键回到今日。
- **当日议程**：按时间列出当天所有日程，长列表可滚动。
- **收起态 pill**：可折叠成小胶囊，自动轮播当天日程；鼠标悬停显示完整备注，滚轮可手动切换。
- **定时提醒**：可设置日程开始前提醒，到点系统弹窗通知。
- **循环日程**：支持按天 / 周 / 月 / 年等周期重复。
- **自动清理过期日程**：启动时与运行中每 60 秒巡检；循环事件永不删除，全天事件在结束日次日才移除。
- **文件导入**：支持 `.ics` / `.csv` / `.xlsx` / `.json`，可选择「覆盖」或「追加」，也可拖放文件导入。
- **多语言界面**：支持 中文 / English (US) / English (UK)，默认跟随系统；设置中切换后即时生效（日期格式、12/24 小时制、每周起始日随语言习惯变化）。
- **开机自启**、单实例运行、高 DPI 自适应、深色主题。

### 目录结构

```
schedule/
├── src/                  全部源码与资源（.c/.h/.rc/.ico/.manifest）
├── third_party/cJSON/    第三方 JSON 解析库（MIT，含 LICENSE）
├── tools/                自测程序与测试构建脚本
├── build.bat             一键构建脚本（自动探测 MSVC）
├── LICENSE               项目许可（Mozilla Public License 2.0）
├── .clangd               clangd 编辑器配置
├── compile_flags.txt     clangd 编译标志
└── .gitignore
```

### 环境要求

- **Windows 10 / 11**（x64）
- **Visual Studio 2019 或更高版本**（需勾选「使用 C++ 的桌面开发」，提供 MSVC 编译器与 Windows SDK）
  - 也支持仅安装 [Visual Studio Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/)。
- 构建脚本会通过 `vswhere` **自动查找** VS 安装路径，无需手工配置；命令行构建不依赖 IDE。

### 一键构建

在项目根目录双击或在命令行执行：

```bat
build.bat
```

成功后产物位于：

```
release\ScheduleWidget.exe
```

直接双击该 exe 即可运行（无需安装、无需附带 dll）。

> 如果脚本提示找不到 vcvars64.bat，请确认已安装 MSVC C++ 工具链；也可在「x64 Native Tools Command Prompt for VS」中手动执行 `build.bat`。

### 使用说明

运行后窗口出现在屏幕右下角，右键窗口或点击右上角「⋯」打开菜单：

| 操作 | 说明 |
|---|---|
| 新建日程 | 编辑标题、日期/时间、备注、颜色、提醒、重复规则 |
| 导入日程文件… | 选择或拖入 ics/csv/xlsx/json；已有日程时可选**覆盖**或**追加** |
| 导出日程… | 导出为 JSON 备份 |
| 恢复备份（JSON）… | 从之前导出的 JSON 恢复 |
| 开机自启 | 切换登录 Windows 后自动运行 |
| 清除所有日程 | 清空（不可撤销，操作前建议先导出备份） |

交互细节：

- **收起态**：点击「—」折叠为胶囊；悬停可看完整备注，**滚轮上下**切换当天不同日程。
- **回到今日**：翻到其他月份或选中其他日期后，月份行会出现「今」按钮，点击即回到今天。
- **导入窗口的「示范文件」**按钮会生成 4 种格式的示例文件，可对照格式编辑后再导入。

### 导入文件格式

- **CSV**：建议表头为 `日期,时间,标题,备注`（表头可识别中英文，列顺序不限）；时间列留空即视为**全天日程**；兼容 UTF-8 BOM。
- **XLSX**：按第一张工作表、与 CSV 相同的列约定解析。
- **ICS**：标准 iCalendar（iCal）格式。
- **JSON**：日程数组，或 `{"events":[...]}` / `{"items":[...]}`；字段支持 `date`/`time`/`title`/`note`，也兼容 `start`/`summary`/`description` 等常见命名。

### 输入输出示例

**输入 — CSV**（`日程示范.csv`，UTF-8 带 BOM；时间留空 = 全天）：

```csv
日期,时间,标题,备注
2026-09-21,,项目评审,3 楼会议室
2026-09-21,14:30,团队午餐,餐厅二楼
2026-10-06,09:00,季度汇报,会议室 A
```

**输入 — ICS**（`日程示范.ics`，`VALUE=DATE` 为全天日程）：

```ics
BEGIN:VCALENDAR
VERSION:2.0
PRODID:-//ScheduleWidget//Sample//CN
BEGIN:VEVENT
UID:sample-1@schedulewidget
DTSTAMP:20260101T000000Z
DTSTART;VALUE=DATE:20260921
SUMMARY:项目评审
DESCRIPTION:3 楼会议室
END:VEVENT
END:VCALENDAR
```

**输入 — JSON**（`日程示范.json`）：

```json
[
  { "date": "2026-09-21", "title": "项目评审", "desc": "3 楼会议室" },
  { "date": "2026-09-21", "time": "14:30", "title": "团队午餐", "desc": "餐厅二楼" }
]
```

**输出 — 导出/备份 JSON**（「导出日程…」生成的 `schedule.json` 结构，导入 `.json` 时也兼容此格式）：

```json
{
  "events": [
    {
      "id": "8f3c…",
      "title": "团队午餐",
      "note": "餐厅二楼",
      "start": "2026-09-21T14:30",
      "end": "2026-09-21T15:30",
      "allDay": false,
      "color": 2,
      "remind": 30,
      "rec": "none",
      "interval": 1,
      "unit": "day",
      "until": ""
    },
    {
      "id": "a1d9…",
      "title": "项目评审",
      "note": "3 楼会议室",
      "start": "2026-09-21",
      "end": "2026-09-21",
      "allDay": true,
      "color": 0,
      "remind": -1,
      "rec": "weekly",
      "interval": 1,
      "unit": "week",
      "until": "2026-12-31"
    }
  ]
}
```

字段说明：定时事件 `start`/`end` 为 `YYYY-MM-DDTHH:MM`；全天事件为纯日期 `YYYY-MM-DD`（`end` 含当天）；`remind` 为提前分钟数（`-1` 不提醒）；`rec` ∈ `none/daily/weekly/monthly/yearly/custom`；`unit` ∈ `minute/hour/day/week/month/year`（`rec` 为 `custom` 时与 `interval` 搭配使用）。

**输出 — 界面显示**（中文 / English (US) 的格式差异）：

| 场景 | 中文 | English (US) |
|---|---|---|
| 定时日程胶囊（未来） | `9月21日 14:30 · 30分钟后` | `9/21 2:30 PM · in 30 minutes` |
| 全天日程胶囊（无备注） | `9月21日 · 3天后` | `9/21 · in 3 days` |
| 议程时间范围（同日定时） | `14:30 – 15:30` | `2:30 PM – 3:30 PM` |
| 全天跨日范围 | `全天 · 9月21日 ~ 9月22日` | `All day · 9/21 ~ 9/22` |

> English (UK) 与中文同为 24 小时制与周一起始，日期为 `21/9/2026` 风格；English (US) 为 12 小时制、周日起始。

> 点导入窗口的「示范文件」按钮可一键生成这四种格式的真实文件（中英文内容随界面语言自动切换），直接对照编辑即可。

### 常见问题

- **导入 .doc / .xls 没反应？** 旧版二进制格式不支持，请先另存为 `.docx` / `.xlsx` / `.csv` 再导入。
- **CSV 导入后标题/备注错位？** 请确认首行为表头（`日期,时间,标题,备注` 或 `date,time,title,note`），文件保存为 UTF-8（带 BOM 更稳）。
- **误清空了日程怎么办？** 「清除所有日程」不可撤销；养成定期「导出日程…」备份的习惯，恢复用「恢复备份（JSON）…」。
- **窗口被其他窗口挡住？** 组件钉在桌面层（壁纸之上、普通窗口之下），正常使用不受影响；在「显示桌面」后它会再次露出。
- **换了语言没全部生效？** 菜单、气泡即时切换；已打开的导入窗口需关闭后重新打开。

### 数据存储位置

所有数据保存在当前用户目录，卸载/删除 exe 不会自动清除：

```
%APPDATA%\ScheduleWidget\
├── schedule.json     日程数据（{"events":[...]}）
└── settings.json     设置（开机自启、语言、默认提醒等）
```

如需迁移或备份，直接复制整个 `ScheduleWidget` 文件夹即可。

### 运行自测

`tools/` 下包含 5 套自测（导入解析、全天事件、过期清理、xlsx 文本抽取、示范文件生成）。例如：

```bat
tools\build_test.bat test_purge build\util.obj build\lang.obj
```

脚本会自动编译并运行对应测试，输出 ALL PASS 即通过；第 2 个及之后的参数是要一起链接的主工程 obj（先执行一次 `build.bat` 生成）。各测试所需 obj：

| 测试 | 额外 obj |
|---|---|
| test_import | `build\lang.obj build\util.obj` |
| test_purge / test_allday | `build\util.obj build\lang.obj` |
| test_extract / test_samples | `build\text_extract.obj build\lang.obj build\util.obj` |

### 第三方组件

- [cJSON](https://github.com/DaveGamble/cJSON)（MIT License），见 `third_party/cJSON/LICENSE`。

### 许可

本项目源码以 [Mozilla Public License 2.0](https://mozilla.org/MPL/2.0/)（MPL 2.0）发布，完整文本见根目录 `LICENSE`。

- 可自由使用、修改、分发（含商用），需保留版权与许可声明。
- 修改本项目的源码文件须以 MPL 2.0 开源；与之结合的其他文件（Larger Work）可使用任意许可。
- 本项目不提供任何担保，作者不承担因使用产生的任何责任。

---

## English

A lightweight Windows desktop schedule widget written in pure C + Win32 API + GDI+, with zero runtime dependencies (statically linked `/MT`, runs as a single exe). Pinned to the bottom-right corner of your desktop, it provides a monthly calendar, daily agenda, timed reminders, and schedule file import.

### What Problem Does It Solve

- **Want to check your schedule without opening an app**: Calendar/todo apps are slow to launch and cluttered with popups; this widget sits on the desktop layer — just glance at the calendar and today's agenda anytime, and it collapses into a tiny pill when not needed.
- **Schedules scattered across sources**: Import everything at once from iCal (.ics), Excel/spreadsheets (.xlsx/.csv), or your own scripts (.json) — no manual re-entry.
- **Afraid of missing things, but also afraid of being disturbed**: Set advance reminders per event; collapse the window into a pill that auto-rotates today's schedule.
- **Tired of install dependencies**: Single exe, no .NET / no runtime libraries, all data stored locally — just copy and go.

### Features

- **Calendar view**: Marks dates with events, highlights today and selected day, prev/next month navigation, "Today" button to jump back.
- **Daily agenda**: Lists all events for the selected day in time order; long lists scroll.
- **Collapsed pill**: Fold into a compact pill that auto-rotates today's events; hover to see full notes, scroll wheel to switch manually.
- **Timed reminders**: Set advance reminders before event start; system toast notification at the scheduled time.
- **Recurring events**: Support for daily / weekly / monthly / yearly / custom interval recurrence.
- **Auto-cleanup of expired events**: Checked at startup and every 60 seconds; recurring events are never deleted, all-day events are removed the day after they end.
- **File import**: Supports `.ics` / `.csv` / `.xlsx` / `.json`; choose "overwrite" or "append"; drag-and-drop supported.
- **Multilingual UI**: Supports Chinese / English (US) / English (UK), defaults to system language; switches instantly in settings (date format, 12/24-hour clock, week start day adapt to language).
- **Autostart**, single-instance, high-DPI aware, dark theme.

### Directory Structure

```
schedule/
├── src/                  All source code and resources (.c/.h/.rc/.ico/.manifest)
├── third_party/cJSON/    Third-party JSON parser (MIT, includes LICENSE)
├── tools/                Self-test programs and test build scripts
├── build.bat             One-click build script (auto-detects MSVC)
├── LICENSE               Project license (Mozilla Public License 2.0)
├── .clangd               clangd editor config
├── compile_flags.txt     clangd compile flags
└── .gitignore
```

### Requirements

- **Windows 10 / 11** (x64)
- **Visual Studio 2019 or later** (select "Desktop development with C++" for MSVC compiler and Windows SDK)
  - Alternatively, install only [Visual Studio Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/).
- The build script **auto-detects** the VS installation path via `vswhere`; no manual configuration needed.

### One-Click Build

Double-click or run from the command line in the project root:

```bat
build.bat
```

The output is located at:

```
release\ScheduleWidget.exe
```

Double-click the exe to run — no installation, no additional DLLs required.

> If the script cannot find vcvars64.bat, ensure MSVC C++ toolchain is installed; you can also run `build.bat` manually from "x64 Native Tools Command Prompt for VS".

### Usage

After launching, the window appears at the bottom-right corner of the screen. Right-click the window or click the "⋯" button in the top-right to open the menu:

| Action | Description |
|---|---|
| New Event | Edit title, date/time, note, color, reminder, recurrence rule |
| Import Schedule File… | Select or drag in ics/csv/xlsx/json; choose **overwrite** or **append** if events exist |
| Export Schedule… | Export as JSON backup |
| Restore Backup (JSON)… | Restore from a previously exported JSON |
| Autostart | Toggle auto-launch on Windows login |
| Clear All Events | Wipe all events (irreversible — export a backup first) |

Interaction tips:

- **Collapsed state**: Click "—" to fold into a pill; hover to see full notes, **scroll wheel** to switch between today's events.
- **Back to today**: After navigating to another month or selecting another date, a "Today" button appears in the month header — click to return.
- The **"Sample Files"** button in the import window generates 4 example files for each format — use them as templates.

### Import File Formats

- **CSV**: Recommended header `日期,时间,标题,备注` or `date,time,title,note` (headers are locale-aware, column order is flexible); empty time column = all-day event; UTF-8 BOM compatible.
- **XLSX**: Parsed using the first worksheet, same column conventions as CSV.
- **ICS**: Standard iCalendar (iCal) format.
- **JSON**: Event array, or `{"events":[...]}` / `{"items":[...]}`; fields support `date`/`time`/`title`/`note`, also compatible with `start`/`summary`/`description` and other common names.

### Input/Output Examples

**Input — CSV** (`schedule_sample.csv`, UTF-8 with BOM; empty time = all-day):

```csv
date,time,title,note
2026-09-21,,Project Review,3rd Floor Conference Room
2026-09-21,14:30,Team Lunch,2nd Floor Cafeteria
2026-10-06,09:00,Quarterly Report,Conference Room A
```

**Input — ICS** (`schedule_sample.ics`, `VALUE=DATE` for all-day):

```ics
BEGIN:VCALENDAR
VERSION:2.0
PRODID:-//ScheduleWidget//Sample//EN
BEGIN:VEVENT
UID:sample-1@schedulewidget
DTSTAMP:20260101T000000Z
DTSTART;VALUE=DATE:20260921
SUMMARY:Project Review
DESCRIPTION:3rd Floor Conference Room
END:VEVENT
END:VCALENDAR
```

**Input — JSON** (`schedule_sample.json`):

```json
[
  { "date": "2026-09-21", "title": "Project Review", "note": "3rd Floor Conference Room" },
  { "date": "2026-09-21", "time": "14:30", "title": "Team Lunch", "note": "2nd Floor Cafeteria" }
]
```

**Output — Export/Backup JSON** (structure generated by "Export Schedule…"; also compatible with `.json` import):

```json
{
  "events": [
    {
      "id": "8f3c…",
      "title": "Team Lunch",
      "note": "2nd Floor Cafeteria",
      "start": "2026-09-21T14:30",
      "end": "2026-09-21T15:30",
      "allDay": false,
      "color": 2,
      "remind": 30,
      "rec": "none",
      "interval": 1,
      "unit": "day",
      "until": ""
    },
    {
      "id": "a1d9…",
      "title": "Project Review",
      "note": "3rd Floor Conference Room",
      "start": "2026-09-21",
      "end": "2026-09-21",
      "allDay": true,
      "color": 0,
      "remind": -1,
      "rec": "weekly",
      "interval": 1,
      "unit": "week",
      "until": "2026-12-31"
    }
  ]
}
```

Field reference: timed events use `start`/`end` as `YYYY-MM-DDTHH:MM`; all-day events use plain date `YYYY-MM-DD` (`end` is inclusive); `remind` is minutes before start (`-1` = no reminder); `rec` ∈ `none/daily/weekly/monthly/yearly/custom`; `unit` ∈ `minute/hour/day/week/month/year` (used with `interval` when `rec` is `custom`).

**Output — UI Display** (format differences between Chinese and English (US)):

| Scenario | Chinese | English (US) |
|---|---|---|
| Timed event pill (upcoming) | `9月21日 14:30 · 30分钟后` | `9/21 2:30 PM · in 30 minutes` |
| All-day event pill (no note) | `9月21日 · 3天后` | `9/21 · in 3 days` |
| Agenda time range (same day) | `14:30 – 15:30` | `2:30 PM – 3:30 PM` |
| All-day multi-day range | `全天 · 9月21日 ~ 9月22日` | `All day · 9/21 ~ 9/22` |

> English (UK) shares 24-hour clock and Monday-start week with Chinese, but uses `21/9/2026` date format; English (US) uses 12-hour clock with Sunday-start week.

> Click the "Sample Files" button in the import window to generate real example files in all four formats (content language follows the UI language).

### FAQ

- **Can't import .doc / .xls files?** Legacy binary formats are not supported — save as `.docx` / `.xlsx` / `.csv` first.
- **CSV import shows misaligned columns?** Ensure the first row is a header (`date,time,title,note`), and the file is UTF-8 encoded (BOM recommended).
- **Accidentally cleared all events?** "Clear All Events" is irreversible; make a habit of exporting backups regularly via "Export Schedule…", and restore with "Restore Backup (JSON)…".
- **Window hidden behind other windows?** The widget is pinned to the desktop layer (above wallpaper, below normal windows); it's normally unaffected, and reappears after "Show Desktop".
- **Language switch not fully applied?** Menus and notifications switch instantly; an already-open import window needs to be closed and reopened.

### Data Storage Location

All data is stored in the current user's directory; uninstalling or deleting the exe does not auto-clean:

```
%APPDATA%\ScheduleWidget\
├── schedule.json     Event data ({"events":[...]})
└── settings.json     Settings (autostart, language, default reminder, etc.)
```

To migrate or back up, simply copy the entire `ScheduleWidget` folder.

### Running Self-Tests

The `tools/` directory contains 5 self-test suites (import parsing, all-day events, expiry cleanup, xlsx text extraction, sample file generation). For example:

```bat
tools\build_test.bat test_purge build\util.obj build\lang.obj
```

The script compiles and runs the test; "ALL PASS" means success. The 2nd and later arguments are main-project objs to link (run `build.bat` once first to generate them). Required objs per test:

| Test | Extra objs |
|---|---|
| test_import | `build\lang.obj build\util.obj` |
| test_purge / test_allday | `build\util.obj build\lang.obj` |
| test_extract / test_samples | `build\text_extract.obj build\lang.obj build\util.obj` |

### Third-Party Components

- [cJSON](https://github.com/DaveGamble/cJSON) (MIT License), see `third_party/cJSON/LICENSE`.

### License

This project's source code is licensed under [Mozilla Public License 2.0](https://mozilla.org/MPL/2.0/) (MPL 2.0). See the `LICENSE` file in the root directory for the full text.

- You are free to use, modify, and distribute (including commercially), provided you retain copyright and license notices.
- Modified source files of this project must remain under MPL 2.0; other files combined into a Larger Work may use any license.
- This project is provided "as is" without any warranty; the author assumes no liability for damages arising from its use.
