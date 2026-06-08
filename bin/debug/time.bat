@echo off
setlocal

for /f %%a in ('powershell -command "[DateTime]::UtcNow.Ticks"') do set START=%%a

.\Kura.exe run.kura

for /f %%a in ('powershell -command "[DateTime]::UtcNow.Ticks"') do set END=%%a

powershell -command "$elapsed=([Int64]%END% - [Int64]%START%) / 10000000.0; Write-Host ('Execution time: {0:N3} seconds' -f $elapsed)"

pause