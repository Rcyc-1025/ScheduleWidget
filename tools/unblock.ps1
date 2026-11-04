<#
.SYNOPSIS
    Removes the Mark-of-the-Web (Zone.Identifier) from ScheduleWidget.exe so
    Windows SmartScreen no longer treats it as a downloaded file.

.PARAMETER Path
    Path to ScheduleWidget.exe. Defaults to ..\release\ScheduleWidget.exe.

.PARAMETER Launch
    Start the executable after unblocking it.

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File tools\unblock.ps1 -Path .\ScheduleWidget.exe

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File tools\unblock.ps1 -Path .\ScheduleWidget.exe -Launch
#>
[CmdletBinding()]
param(
    [string]$Path = (Join-Path (Split-Path -Parent $PSScriptRoot) 'release\ScheduleWidget.exe'),
    [switch]$Launch
)

$ErrorActionPreference = 'Stop'
$exe = [System.IO.Path]::GetFullPath($Path)

if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) {
    Write-Error "File not found: $exe"
    exit 1
}

try {
    Unblock-File -LiteralPath $exe -ErrorAction Stop
}
catch {
    Write-Error "Failed to unblock $exe : $($_.Exception.Message)"
    exit 1
}

$zone = Get-Item -LiteralPath $exe -Stream Zone.Identifier -ErrorAction SilentlyContinue
if ($null -ne $zone) {
    Write-Warning "Zone.Identifier is still present on $exe"
    exit 1
}

Write-Host "Unblocked: $exe"

if ($Launch) {
    Start-Process -FilePath $exe
}
