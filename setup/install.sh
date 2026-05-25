#!/bin/bash

distro=$(cat /proc/version)
ubuntu=1
echo $distro | grep -i 'ubuntu'
if [ $? -eq 1 ]; then
    echo $distro | grep -i 'debian'
    if [ $? -eq 1 ]; then
        echo === This script is intended for Ubuntu and Debian distributions only ===
        exit
    else
        ubuntu=0
    fi
fi

if [[ "$(pwd)" != *setup ]]; then
    echo "=== This script needs to be run from the repository's setup folder ==="
    exit
fi

echo === Installing build tools and libcod2 dependencies ===
sudo dpkg --add-architecture i386
sudo apt update
if [ $ubuntu -eq 1 ]; then
    sudo apt install -y gcc-multilib g++-multilib libstdc++5:i386 libmysqlclient-dev:i386 
else
    sudo apt install -y gcc-multilib g++-multilib libmariadb-dev-compat:i386
    if dpkg --compare-versions $(cat /etc/debian_version) ge 13
    then
        sudo apt install -y wget
        wget http://ftp.debian.org/debian/pool/main/g/gcc-3.3/libstdc++5_3.3.6-32_i386.deb
        sudo dpkg -i libstdc++5_3.3.6-32_i386.deb
        rm libstdc++5_3.3.6-32_i386.deb
    else
        sudo apt install -y libstdc++5:i386
    fi
fi

echo === Installing Speex dependencies ===
sudo apt install -y ffmpeg git autoconf automake libtool build-essential libogg-dev libogg-dev:i386
cd ../../ && git clone https://gitlab.xiph.org/xiph/speex.git
cd speex
git checkout tags/Speex-1.1.9 -b 1.1.9

echo === Building Speex ===
env AUTOMAKE=automake ACLOCAL=aclocal LIBTOOLIZE=libtoolize ./autogen.sh CFLAGS=-m32 CXXFLAGS=-m32 LDFLAGS=-m32 --build=x86_64-pc-linux-gnu --host=i686-pc-linux-gnu
make
sudo make install
sudo ldconfig
cd ..
rm -rf ./speex

echo === Building libcod2 ===
cd ./zk_libcod/code
./doit.sh mysql1

echo === Installation done ===
echo === Modify setup/run.sh to your needs ===