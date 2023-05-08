Installing Speex 1.1.9 (for 64-bit Ubuntu 22.04 LTS):
```
# Get the code:
git clone https://gitlab.xiph.org/xiph/speex.git
cd speex
git checkout tags/Speex-1.1.9 -b 1.1.9

# Install the build environment/dependencies:
sudo apt update
sudo apt install autoconf automake libtool build-essential libogg-dev libogg-dev:i386

# Generate the build configuration:
env AUTOMAKE=automake ACLOCAL=aclocal LIBTOOLIZE=libtoolize ./autogen.sh CFLAGS=-m32 CXXFLAGS=-m32 LDFLAGS=-m32 --build=x86_64-pc-linux-gnu --host=i686-pc-linux-gnu

# Compile the code:
# The first two commands will fail on a 64-bit system unless 32-bit Ogg libraries are installed
# However, linking with libcod succeeds anyway since we do not need speexdec and speexenc
make
sudo make install
sudo ldconfig

# Optional: Clean/delete the build environment
cd ..
rm -rf ./speex
```