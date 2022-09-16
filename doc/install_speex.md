Installing Speex 1.1.9 (for 64-bit Ubuntu 18.04.5 LTS):
```
# Get the code:
git clone https://gitlab.xiph.org/xiph/speex.git
cd speex
git checkout tags/Speex-1.1.9 -b 1.1.9

# Install the build environment/dependencies:
sudo apt update
sudo apt install autoconf libtool build-essential

# Adapt the autogen.sh file so that the following three tools are detected correctly:
# You might first make sure that they're actually installed/present
    AUTOMAKE=automake
    ACLOCAL=aclocal
    LIBTOOLIZE=libtoolize

# Generate the build configuration:
./autogen.sh CFLAGS=-m32 CXXFLAGS=-m32 LDFLAGS=-m32 --build=x86_64-pc-linux-gnu --host=i686-pc-linux-gnu

# Compile the code:
# The following two commands will fail on a 64-bit system unless 32-bit Ogg libraries are installed
# However, linking with libcod succeeds anyway since we don't need speexdec and speexenc
make
sudo make install
sudo ldconfig
```