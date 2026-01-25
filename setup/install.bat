@echo off
setlocal

set distro=Ubuntu-24.04

net session >nul 2>&1
if %errorLevel% neq 0 (
    echo === This script needs to be run as Administrator ===
    goto end
)

echo === Checking current WSL setup ===
dism.exe /english /online /get-featureinfo /featurename:VirtualMachinePlatform | findstr "State\ \:\ Enabled" >nul 2>&1
if %errorLevel% neq 0 (
    echo === Enabling Virtual Machine Platform feature ===
    dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
)

dism.exe /english /online /get-featureinfo /featurename:Microsoft-Windows-Subsystem-Linux | findstr "State\ \:\ Enabled" >nul 2>&1
if %errorLevel% neq 0 (
    echo === Enabling WSL feature ===
    dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
)

set WSL_UTF8=1
wsl --update
wsl --list | findstr %distro% >nul 2>&1
if %errorLevel% neq 0 (
    echo === Setting up WSL with %distro% ===
    echo === On first use, WSL will prompt for username/password after Ubuntu is installed ===
    echo === Those need to be set before the installation can proceed ===
    echo === Once that is done, leave the WSL shell with 'exit' ===
    pause
    wsl --install -d %distro%
)

echo === Testing WSL ===
wsl -d %distro% -- whoami >nul 2>&1
if %errorLevel% == -1 (
    echo === Check output, system may need a reboot or BIOS change to continue WSL installation ===
    goto end
)
if %errorLevel% neq 0 (
    echo === WSL encountered an error ===
    wsl -d %distro% -- whoami
    goto end
)
for /f %%a in ('wsl -d %distro% -- whoami') do set wsluser=%%a

echo === Installing build tools and libcod2 dependencies ===
wsl -d %distro% -- sudo dpkg --add-architecture i386 ^&^& ^
sudo apt update ^&^& ^
sudo apt install -y gcc-multilib g++-multilib libstdc++5:i386 libmysqlclient-dev:i386 

set speex=nospeex
set /P INSTALLSPEEX=Install realtime sound streaming support (Y/[N])? 
if /I "%INSTALLSPEEX%" neq "Y" goto libcod
set speex=

echo === Building and installing Speex ===
wsl -d %distro% -- sudo apt install -y git ffmpeg autoconf automake libtool build-essential libogg-dev libogg-dev:i386
wsl -d %distro% --cd ~/ -- ^
git clone https://gitlab.xiph.org/xiph/speex.git ^&^& ^
cd ./speex ^&^& ^
git checkout tags/Speex-1.1.9 -b 1.1.9 ^&^& ^
env AUTOMAKE=automake ACLOCAL=aclocal LIBTOOLIZE=libtoolize ./autogen.sh CFLAGS=-m32 CXXFLAGS=-m32 LDFLAGS=-m32 --build=x86_64-pc-linux-gnu --host=i686-pc-linux-gnu ^&^& ^
make ^&^& ^
sudo make install ^&^& ^
sudo ldconfig ^&^& ^
cd .. ^&^& rm -rf ./speex

:libcod
echo === Building libcod2 ===
wsl -d %distro% --cd ~/ -- ^
git clone https://github.com/ibuddieat/zk_libcod.git ^&^& ^
cd ./zk_libcod/code ^&^& ^
./doit.sh mysql1 %speex%

echo === Installation done ===
echo === Modify setup/run.sh to your needs ===
start \\wsl.localhost\%distro%\home\%wsluser%\zk_libcod\setup\run.sh

:end
endlocal
pause