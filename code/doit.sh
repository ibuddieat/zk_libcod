#!/bin/bash

# Examples:
# ./doit.sh clean
# ./doit.sh debug
# ./doit.sh nospeex
# ./doit.sh nospeex debug
# ./doit.sh nomysql
# ./doit.sh nomysql nospeex debug
# ./doit.sh mysql1 nospeex debug
# ./doit.sh mysql2
# ./doit.sh http             (httpFetch + webSocket; needs cmake, mbedTLS & libwebsockets submodules)
# ./doit.sh http debug
# ./doit.sh mysql1 http
# ./doit.sh mysql2 http nospeex
# ./doit.sh mysql1 http nospeex debug

# Exit on compiler error, with non-zero exit code
set -e

# Obtain commit hash
if command -v git >/dev/null 2>&1 && git rev-parse --git-dir >/dev/null 2>&1; then
    GIT_HASH=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
	# Mark hash if there are uncommitted changes
    if ! git diff --quiet 2>/dev/null; then
        GIT_HASH="${GIT_HASH}-dirty"
    fi
else
    GIT_HASH="unknown"
fi

# Compiler options
cc="g++"
options="-I. -m32 -fPIC -Wall"

# Integrate MySQL? Which variant?
mysql_variant=0
if [ "$1" != "clean" ] && [ "$1" != "nomysql" ] && [ "$2" != "nomysql" ] && [ "$3" != "nomysql" ]; then
	if [ "$1" == "mysql1" ] || [ "$2" == "mysql1" ] || [ "$3" == "mysql1" ]; then
		key='1'
	elif [ "$1" == "mysql2" ] || [ "$2" == "mysql2" ] || [ "$3" == "mysql2" ]; then
		key='2'
	else
		read -rsp $'\nChoose Your MySQL variant:\n
		0. MySQL disabled. (default)\n
		1. Default MySQL variant: A classic MySQL implementation
		made by kungfooman and IzNoGoD. Multiple connections, multiple threads,
		good for servers that use remote MySQL sessions, IRC stuff, and etc.\n
		2. VoroN\'s MySQL variant (his own MySQL implementation). Native
		callbacks, native arguments, single connection, single thread, good
		for local MySQL session, less cpu usage, less memory usage.\n
		Press a key to continue...\n' -n1 key
	fi

	if [ "$key" = '1' ]; then
		mysql_variant=1
		sed -i "/#define COMPILE_MYSQL_DEFAULT 0/c\#define COMPILE_MYSQL_DEFAULT 1" config.hpp
		if [ -d "./vendors/lib" ]; then
			mysql_link="-lmysqlclient -L./vendors/lib"
			export LD_LIBRARY_PATH_32="./vendors/lib"
		else
			mysql_link="-lmysqlclient -L/usr/lib/mysql"
		fi
	elif [ "$key" = '2' ]; then
		mysql_variant=2
		sed -i "/#define COMPILE_MYSQL_VORON 0/c\#define COMPILE_MYSQL_VORON 1" config.hpp
		if [ -d "./vendors/lib" ]; then
			mysql_link="-lmysqlclient -L./vendors/lib"
			export LD_LIBRARY_PATH_32="./vendors/lib"
		else
			mysql_link="-lmysqlclient -L/usr/lib/mysql"
		fi
	else
		mysql_link=""
		mysql_variant=0
	fi
fi

if [ "$1" == "clean" ]; then
	echo "##### CLEAN OBJECTS #####"
	rm objects_* -rf
	rm bin -rf
	exit 1
else
	if [ "$1" == "nomysql" ] || [ "$2" == "nomysql" ] || [ "$3" == "nomysql" ] || [ "$4" == "nomysql" ]; then
		mysql_link=""
		mysql_variant=0
	fi

	if [ "$1" == "nospeex" ] || [ "$2" == "nospeex" ] || [ "$3" == "nospeex" ] || [ "$4" == "nospeex" ]; then
		speex_link=""
		sed -i "/#define COMPILE_SPEEX 1/c\#define COMPILE_SPEEX 0" config.hpp
	else
		speex_link="-lspeex"
		sed -i "/#define COMPILE_SPEEX 0/c\#define COMPILE_SPEEX 1" config.hpp
	fi

	if [ "$1" == "unsafe" ] || [ "$2" == "unsafe" ] || [ "$3" == "unsafe" ] || [ "$4" == "unsafe" ]; then
		sed -i "/#define ENABLE_UNSAFE 0/c\#define ENABLE_UNSAFE 1" config.hpp
	else
		sed -i "/#define ENABLE_UNSAFE 1/c\#define ENABLE_UNSAFE 0" config.hpp
	fi

	# "http" enables httpFetch and webSocket (reset to off after linking)
	net_flag=0
	for a in "$1" "$2" "$3" "$4"; do
		if [ "$a" == "http" ]; then
			net_flag=1
		fi
	done
	if [ "$net_flag" == "1" ]; then
		sed -i "/#define COMPILE_HTTP 0/c\#define COMPILE_HTTP 1" config.hpp
	fi

	if [ "$1" == "debug" ] || [ "$2" == "debug" ] || [ "$3" == "debug" ]; then
		debug="-g -ggdb -O0"
	else
		debug=""
	fi
	set -- "cod2"
fi

mkdir -p bin
mkdir -p objects_$1

echo "##### COMPILE $1 CRACKING.CPP #####"
$cc $debug $options $constants -c cracking.cpp -o objects_$1/cracking.opp

echo "##### COMPILE $1 GSC.CPP #####"
$cc $debug $options $constants -c gsc.cpp -o objects_$1/gsc.opp

if  grep -q "COMPILE_BOTS 1" config.hpp; then
	echo "##### COMPILE $1 GSC_BOTS.CPP #####"
	$cc $debug $options $constants -c gsc_bots.cpp -o objects_$1/gsc_bots.opp
fi

if  grep -q "COMPILE_ENTITY 1" config.hpp; then
	echo "##### COMPILE $1 GSC_ENTITY.CPP #####"
	$cc $debug $options $constants -c gsc_entity.cpp -o objects_$1/gsc_entity.opp
fi

if grep -q "COMPILE_EXEC 1" config.hpp; then
	echo "##### COMPILE $1 GSC_EXEC.CPP #####"
	$cc $debug $options $constants -c gsc_exec.cpp -o objects_$1/gsc_exec.opp
fi

if grep -q "COMPILE_GRAPH 1" config.hpp; then
	echo "##### COMPILE $1 GSC_GRAPH.CPP #####"
	$cc $debug $options $constants -c gsc_graph.cpp -o objects_$1/gsc_graph.opp
fi

if grep -q "COMPILE_JSON 1" config.hpp; then
	# yyjson: i386 SSE math, drop incremental reader + utils, hide symbols.
	echo "##### COMPILE $1 YYJSON.C #####"
	$cc $debug $options $constants -mfpmath=sse -msse2 -DYYJSON_DISABLE_INCR_READER=1 -DYYJSON_DISABLE_UTILS=1 -fvisibility=hidden -w -c lib/yyjson.c -o objects_$1/yyjson.opp
	echo "##### COMPILE $1 GSC_JSON.CPP #####"
	$cc $debug $options $constants -c gsc_json.cpp -o objects_$1/gsc_json.opp
fi

if grep -q "COMPILE_LEVEL 1" config.hpp; then
	echo "##### COMPILE $1 GSC_LEVEL.CPP #####"
	$cc $debug $options $constants -c gsc_level.cpp -o objects_$1/gsc_level.opp
fi

if grep -q "COMPILE_MEMORY 1" config.hpp; then
	echo "##### COMPILE $1 GSC_MEMORY.CPP #####"
	$cc $debug $options $constants -c gsc_memory.cpp -o objects_$1/gsc_memory.opp
fi

if [ $mysql_variant == 1 ]; then
	echo "##### COMPILE $1 GSC_MYSQL.CPP #####"
	$cc $debug $options $constants -c gsc_mysql.cpp -o objects_$1/gsc_mysql.opp
fi

if [ $mysql_variant == 2 ]; then
	echo "##### COMPILE $1 GSC_MYSQL_VORON.CPP #####"
	$cc $debug $options $constants -c gsc_mysql_voron.cpp -o objects_$1/gsc_mysql_voron.opp
fi

if grep -q "COMPILE_PLAYER 1" config.hpp; then
	echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
	$cc $debug $options $constants -c gsc_player.cpp -o objects_$1/gsc_player.opp
fi

if grep -q "COMPILE_UTILS 1" config.hpp; then
	echo "##### COMPILE $1 GSC_UTILS.CPP #####"
	$cc $debug $options $constants -c gsc_utils.cpp -o objects_$1/gsc_utils.opp
fi

if grep -q "COMPILE_WEAPONS 1" config.hpp; then
	echo "##### COMPILE $1 GSC_WEAPONS.CPP #####"
	$cc $debug $options $constants -c gsc_weapons.cpp -o objects_$1/gsc_weapons.opp
fi

if grep -q "COMPILE_HTTP 1" config.hpp; then
	# libwebsockets (v5.0-stable) and mbedTLS (4.1 LTS) are submodules, built static -m32
	# and cached in their _build folders.
	# mbedTLS from git needs python3 + python3-jinja2 + python3-jsonschema to generate sources.
	mbed_src=lib/mbedtls
	mbed_build=$mbed_src/_build
	mbed_libs="$mbed_build/library/libmbedtls.a $mbed_build/library/libmbedx509.a $mbed_build/tf-psa-crypto/core/libtfpsacrypto.a"
	lws_src=lib/libwebsockets
	lws_build=$lws_src/_build
	lws_a=$lws_build/lib/libwebsockets.a
	# Check out the pinned commits: a plain clone has none, a pull may move a pin.
	# mbedTLS needs its nested submodules, lws's are only tests.
	top="$(git rev-parse --show-toplevel 2>/dev/null || true)"
	if [ -n "$top" ]; then
		git -C "$top" submodule update --init --recursive code/lib/mbedtls || exit 1
		git -C "$top" submodule update --init code/lib/libwebsockets || exit 1
	fi
	if [ ! -f "$mbed_src/tf-psa-crypto/CMakeLists.txt" ] || [ ! -f "$lws_src/CMakeLists.txt" ]; then
		echo "ERROR: lib/mbedtls or lib/libwebsockets is empty. Clone with --recursive."
		exit 1
	fi
	# A new pin or a change to this script rebuilds both
	stamp="$(git -C "$mbed_src" rev-parse HEAD 2>/dev/null || true) $(git -C "$lws_src" rev-parse HEAD 2>/dev/null || true) $(cksum < "$0")"
	if [ "$(cat "$lws_build/stamp" 2>/dev/null)" != "$stamp" ]; then
		rm -rf "$mbed_build" "$lws_build"
	fi
	if [ ! -f "$mbed_build/library/libmbedtls.a" ]; then
		echo "##### CMAKE-BUILD MBEDTLS (static, -m32) #####"
		cmake -S "$mbed_src" -B "$mbed_build" \
			-DCMAKE_BUILD_TYPE=Release \
			-DCMAKE_C_FLAGS="-m32 -fPIC" \
			-DENABLE_PROGRAMS=OFF \
			-DENABLE_TESTING=OFF \
			-DMBEDTLS_FATAL_WARNINGS=OFF || exit 1
		cmake --build "$mbed_build" --target mbedtls -j"$(nproc)" || exit 1
		rm -rf "$lws_build"
	fi
	if [ ! -f "$lws_a" ]; then
		echo "##### CMAKE-BUILD LIBWEBSOCKETS (static, -m32) #####"
		mbed_abs="$(cd "$mbed_src" && pwd)"
		# Client only: everything else v5 turns on by default is off.
		# Async DNS keeps getaddrinfo() off the server frame.
		# tls.c needs genhash; digest auth used to imply GENCRYPTO.
		cmake -S "$lws_src" -B "$lws_build" \
			-DCMAKE_BUILD_TYPE=Release \
			-DCMAKE_C_FLAGS="-m32 -fPIC" \
			-DLWS_WITH_STATIC=ON \
			-DLWS_WITH_SHARED=OFF \
			-DLWS_WITHOUT_SERVER=ON \
			-DLWS_WITHOUT_TESTAPPS=ON \
			-DLWS_WITH_MINIMAL_EXAMPLES=OFF \
			-DLWS_WITH_SSL=ON \
			-DLWS_WITH_MBEDTLS=ON \
			-DLWS_MBEDTLS_INCLUDE_DIRS="$mbed_abs/include" \
			-DLWS_MBEDTLS_LIBRARIES="$mbed_abs/_build/library/libmbedtls.a;$mbed_abs/_build/library/libmbedx509.a" \
			-DTFPSACRYPTO_LIBRARY="$mbed_abs/_build/tf-psa-crypto/core/libtfpsacrypto.a" \
			-DLWS_WITH_HTTP2=OFF \
			-DLWS_WITH_HTTP3=OFF \
			-DLWS_WITH_ZLIB=OFF \
			-DLWS_WITHOUT_EXTENSIONS=ON \
			-DLWS_WITH_SYS_ASYNC_DNS=ON \
			-DLWS_WITH_SECURE_STREAMS=OFF \
			-DLWS_WITH_CONMON=OFF \
			-DLWS_WITH_UPNG=OFF \
			-DLWS_WITH_JPEG=OFF \
			-DLWS_WITH_DLO=OFF \
			-DLWS_WITH_LHP=OFF \
			-DLWS_ROLE_RAW_FILE=OFF \
			-DLWS_UNIX_SOCK=OFF \
			-DLWS_WITH_HTTP_BASIC_AUTH=OFF \
			-DLWS_WITH_HTTP_DIGEST_AUTH=OFF \
			-DLWS_WITH_GENCRYPTO=ON \
			-DLWS_WITH_GZINFLATE=OFF \
			-DLWS_WITH_LEJP=OFF \
			-DLWS_WITH_JSONRPC=OFF \
			-DLWS_WITH_STUB=OFF \
			-DLWS_WITH_CBOR_FLOAT=OFF \
			-DLWS_WITH_CACHE_NSCOOKIEJAR=OFF \
			-DLWS_WITH_NETLINK=OFF \
			-DLWS_WITH_WOL=OFF \
			-DLWS_CLIENT_HTTP_PROXYING=OFF \
			-DLWS_WITH_CUSTOM_HEADERS=OFF \
			-DLWS_WITH_EXPORT_LWSTARGETS=OFF || exit 1
		cmake --build "$lws_build" --target websockets -j"$(nproc)" || exit 1
		echo "$stamp" > "$lws_build/stamp"
	fi
	lws_inc="-I$lws_src/include -I$lws_build -I$mbed_src/include -I$mbed_src/tf-psa-crypto/include -I$mbed_src/tf-psa-crypto/drivers/builtin/include"
	lws_link="$lws_a $mbed_libs"

	echo "##### COMPILE $1 GSC_HTTP_WS.CPP #####"
	$cc $debug $options $constants $lws_inc -c gsc_http_ws.cpp -o objects_$1/gsc_http_ws.opp
fi

if [ "$(< config.hpp grep '#define COMPILE_BSP' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 BSP.CPP #####"
	$cc $debug $options $constants -c bsp.cpp -o objects_"$1"/bsp.opp
fi

if [ "$(< config.hpp grep '#define COMPILE_JUMP' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 JUMP.CPP #####"
	$cc $debug $options $constants -c jump.cpp -o objects_"$1"/jump.opp
fi

echo "##### COMPILE $1 DVAR.CPP #####"
$cc $debug $options $constants -c dvar.cpp -o objects_$1/dvar.opp

echo "##### COMPILE $1 LIBCOD.CPP #####"
$cc $debug $options $constants -DGIT_HASH=\"${GIT_HASH}\" -c libcod.cpp -o objects_$1/libcod.opp

echo "##### COMPILE $1 PROXY.C #####"
$cc $debug $options $constants -c proxy/proxy.c -o objects_"$1"/proxy.opp

echo "##### COMPILE $1 QVSNPRINTF.C #####"
$cc $debug $options $constants -c lib/qvsnprintf.c -o objects_"$1"/qvsnprintf.opp

echo "##### COMPILE $1 RATELIMITER.CPP #####"
$cc $debug $options $constants -c ratelimiter.cpp -o objects_"$1"/ratelimiter.opp

echo "##### COMPILE $1 STRCMP_CONSTANT_TIME.C #####"
$cc $debug $options $constants -c lib/strcmp_constant_time.c -o objects_"$1"/strcmp_constant_time.opp

echo "##### COMPILE $1 UTILS.CPP #####"
$cc $debug $options $constants -c utils.cpp -o objects_"$1"/utils.opp

if [ -d extra ]; then
	echo "##### COMPILE $1 EXTRAS #####"
	cd extra
	for F in *.cpp;
	do
		echo "###### COMPILE $1 EXTRA: $F #####"
		$cc $debug $options $constants -c $F -o ../objects_$1/extra_${F%.cpp}.opp;
	done
	cd ..
fi

echo "##### LINKING lib$1.so #####"
objects="$(ls objects_$1/*.opp)"
# Keep static lws/mbedTLS symbols out of the .so's dynamic symbol table
$cc -m32 -shared -L/lib32 -Wl,--exclude-libs,ALL -o bin/lib$1.so -ldl $objects -lpthread $mysql_link $speex_link $lws_link
rm objects_$1 -r

if [ mysql_variant > 0 ]; then
	sed -i "/#define COMPILE_MYSQL_DEFAULT 1/c\#define COMPILE_MYSQL_DEFAULT 0" config.hpp
	sed -i "/#define COMPILE_MYSQL_VORON 1/c\#define COMPILE_MYSQL_VORON 0" config.hpp
fi

# Reset the opt-in http flag so the repo's config.hpp stays default-off.
if [ "$net_flag" == "1" ]; then
	sed -i "/#define COMPILE_HTTP 1/c\#define COMPILE_HTTP 0" config.hpp
fi

# Read leftover
rm 0
