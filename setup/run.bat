@echo off
setlocal

set distro=Ubuntu-24.04
set port=28960

cd %~dp0
title cod2 server launcher

echo === Getting WSL IP address ===
for /f %%a in ('bash.exe -c "ip address show dev eth0 | grep 'inet ' | cut -d ' ' -f 6 | cut -d '/' -f 1"') do set addr=%%a

set /P EXPOSE=Update firewall to expose server on LAN (Y/[N])? 
if /I "%EXPOSE%" neq "Y" goto nofirewallupdate

net session >nul 2>&1
if %errorLevel% neq 0 (
    echo === This action needs the script to be run as Administrator ===
    goto end
)
echo === Setting up firewall ===
set rulename=Open UDP Port %port% for cod2
netsh advfirewall firewall show rule name="%rulename%" >nul || netsh advfirewall firewall add rule name="%rulename%" dir=in action=allow protocol=UDP localport=%port%
echo === Added firewall rule "%rulename%" ===

:nofirewallupdate
set /P PROXY=Start proxy for connection via LAN ([Y]/N)? 
if /I "%PROXY%" == "N" goto runserverlocal
start "cod2 LAN proxy" cmd /k "echo === Exit with Ctrl-C === && socat\socat.exe -d UDP4-LISTEN:%port%,fork,reuseaddr UDP4-SENDTO:%addr%:%port%"

:runserverlan
for /f "delims=[] tokens=2" %%a in ('ping -4 -n 1 %ComputerName% ^| findstr [') do set lanaddr=%%a
echo === Running server on %distro% ===
echo === LAN connect:   /connect %lanaddr% ===
echo === Local connect: /connect %addr% ===
goto wsl

:runserverlocal
echo === Running server on %distro% ===
echo === Local connect: /connect %addr% ===

:wsl
runas /trustlevel:0x20000 "cmd /k \"title cod2 server ^&^& wsl -d %distro% --cd ~/zk_libcod/setup -- ./run.sh\""

:end
endlocal
pause