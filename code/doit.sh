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
# ./doit.sh http             (httpFetch + webSocket; needs cmake + libmbedtls-dev:i386)
# ./doit.sh http debug
# ./doit.sh mysql1 http
# ./doit.sh mysql2 http nospeex
# ./doit.sh mysql1 http nospeex debug

# Exit on compiler error, with non-zero exit code
set -e

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
		sed -i "/#define COMPILE_CUSTOM_VOICE 1/c\#define COMPILE_CUSTOM_VOICE 0" config.hpp
	else
		speex_link="-lspeex"
		sed -i "/#define COMPILE_CUSTOM_VOICE 0/c\#define COMPILE_CUSTOM_VOICE 1" config.hpp
	fi

	if [ "$1" == "unsafe" ] || [ "$2" == "unsafe" ] || [ "$3" == "unsafe" ] || [ "$4" == "unsafe" ]; then
		sed -i "/#define ENABLE_UNSAFE 0/c\#define ENABLE_UNSAFE 1" config.hpp
	else
		sed -i "/#define ENABLE_UNSAFE 1/c\#define ENABLE_UNSAFE 0" config.hpp
	fi

	# httpFetch and webSocket share one libwebsockets/mbedTLS build, so a single
	# "http" flag enables both (reset to off after linking).
	net_flag=0
	for a in "$1" "$2" "$3" "$4"; do
		if [ "$a" == "http" ]; then
			net_flag=1
		fi
	done
	if [ "$net_flag" == "1" ]; then
		sed -i "/#define COMPILE_HTTP 0/c\#define COMPILE_HTTP 1" config.hpp
		sed -i "/#define COMPILE_WEBSOCKET 0/c\#define COMPILE_WEBSOCKET 1" config.hpp
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

http_on=0; ws_on=0
grep -q "COMPILE_HTTP 1" config.hpp && http_on=1
grep -q "COMPILE_WEBSOCKET 1" config.hpp && ws_on=1

if [ "$http_on" = "1" ] || [ "$ws_on" = "1" ]; then
	# TLS backend: mbedTLS 2.28 from the Ubuntu package (libmbedtls-dev:i386), linked
	# statically (-m32). mbedTLS is ~10x smaller than OpenSSL and has none of OpenSSL
	# 3.x's provider machinery (which crashed in this 32-bit static build). TLS is
	# encrypt-only (no CA), so the .so needs no system cert file. CDN-fronted servers
	# (Cloudflare etc.) work because the client keeps TLS SNI on (LCCSCF_ALLOW_INSECURE,
	# not SKIP_SERVER_CERT_HOSTNAME_CHECK - see http_client.hpp). The libwebsockets .a
	# is cached; remove lib/libwebsockets/_build to force a rebuild.
	lws_src=lib/libwebsockets
	lws_build=$lws_src/_build
	lws_a=$lws_build/lib/libwebsockets.a
	# If a plain `git clone` skipped the submodule, fetch it now so no manual step
	# is needed.
	if [ ! -f "$lws_src/CMakeLists.txt" ]; then
		echo "##### INIT LIBWEBSOCKETS SUBMODULE #####"
		git -C "$(git rev-parse --show-toplevel)" submodule update --init --recursive code/lib/libwebsockets
	fi
	if [ ! -f "$lws_a" ]; then
		echo "##### CMAKE-BUILD LIBWEBSOCKETS (static, -m32, mbedTLS pkg) #####"
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
			-DLWS_MBEDTLS_INCLUDE_DIRS="/usr/include" \
			-DLWS_MBEDTLS_LIBRARIES="/usr/lib/i386-linux-gnu/libmbedtls.a;/usr/lib/i386-linux-gnu/libmbedx509.a;/usr/lib/i386-linux-gnu/libmbedcrypto.a" \
			-DLWS_WITH_HTTP2=OFF \
			-DLWS_WITH_ZLIB=OFF \
			-DLWS_WITHOUT_EXTENSIONS=ON \
			-DLWS_WITH_EXTERNAL_POLL=ON
		cmake --build "$lws_build" --target websockets -j"$(nproc)"
	fi
	lws_inc="-I$lws_src/include -I$lws_build"
	lws_link="$lws_a -l:libmbedtls.a -l:libmbedx509.a -l:libmbedcrypto.a"

	echo "##### COMPILE $1 GSC_EXTRA.CPP #####"
	$cc $debug $options $constants -c gsc_extra.cpp -o objects_$1/gsc_extra.opp
fi

if [ "$http_on" = "1" ]; then
	echo "##### COMPILE $1 GSC_HTTP.CPP #####"
	$cc $debug $options $constants $lws_inc -c gsc_http.cpp -o objects_$1/gsc_http.opp
fi

if [ "$ws_on" = "1" ]; then
	echo "##### COMPILE $1 GSC_WEBSOCKET.CPP #####"
	$cc $debug $options $constants $lws_inc -c gsc_websocket.cpp -o objects_$1/gsc_websocket.opp
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
$cc $debug $options $constants -c libcod.cpp -o objects_$1/libcod.opp

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
$cc -m32 -shared -L/lib32 -o bin/lib$1.so -ldl $objects -lpthread $mysql_link $speex_link $lws_link
rm objects_$1 -r

if [ mysql_variant > 0 ]; then
	sed -i "/#define COMPILE_MYSQL_DEFAULT 1/c\#define COMPILE_MYSQL_DEFAULT 0" config.hpp
	sed -i "/#define COMPILE_MYSQL_VORON 1/c\#define COMPILE_MYSQL_VORON 0" config.hpp
fi

# Reset the opt-in http/ws flags so the repo's config.hpp stays default-off.
if [ "$net_flag" == "1" ]; then
	sed -i "/#define COMPILE_HTTP 1/c\#define COMPILE_HTTP 0" config.hpp
	sed -i "/#define COMPILE_WEBSOCKET 1/c\#define COMPILE_WEBSOCKET 0" config.hpp
fi

# Read leftover
rm 0
