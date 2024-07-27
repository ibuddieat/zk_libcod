## Build environment and supported platforms
- gcc (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0 (with Speex)
- gcc (Debian 10.2.1-6) 10.2.1
- gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0
> [!NOTE]
> This list is not exhaustive and newer versions might work as well.

### Requirements / Dependencies
- gcc and g++ (with multilib on 64-bit x86 operating systems)
- libstdc++5
- MySQL client (if required by functionality)
- Speex 1.1.9 (can be disabled in [config.hpp](code/config.hpp))

**Base requirements installation (for 64-bit Ubuntu 22.04 LTS)**
```
sudo dpkg --add-architecture i386
sudo apt update
sudo apt install gcc-multilib g++-multilib libstdc++5:i386
```

**MySQL requirements installation (for 64-bit Ubuntu 22.04 LTS)**
```
sudo apt install libmysqlclient-dev:i386
```

**MySQL requirements installation (for 64-bit Debian 11.6)**
```
sudo apt install libmariadb-dev-compat:i386
```

**Sound alias duration support requirements installation**
```
sudo apt install ffmpeg
```
If not installed, `getSoundDuration(<aliasname>)` always returns undefined.

**Custom sound file docs (for 64-bit Ubuntu 22.04 LTS)**
- [Speex installation](doc/install_speex.md)
- [Audio file conversion](doc/convert_audio_files.md)
> [!IMPORTANT]
> Note: This feature is enabled by default and increases RAM usage by about 500 MB. See below for how to disable it (`nospeex`).

## Building the binary
```
# Move into  the code directory
cd code

# Normal build
./doit.sh

# Alternative: Without Speex
./doit.sh nospeex

# Alternative: Debug build with Speex
./doit.sh debug
```
On success, this creates `libcod2.so` in `./code/bin`.

## Running your server with libcod
- Short version:
  - `LD_PRELOAD=./bin/libcod2.so ./cod2_lnxded +set fs_game ...` etc.
- The details will not be documented here, please refer to:
  - [Libcod discussion threads at Killtube~](https://killtube.org/forumdisplay.php?44-libcod)
  - [Running a cod2 server within docker, with libcod](https://github.com/rutkowski-tomasz/cod2-docker)