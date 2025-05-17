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

if [ "$(< config.hpp grep '#define COMPILE_BSP' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 BSP.CPP #####"
	$cc $debug $options $constants -c bsp.cpp -o objects_"$1"/bsp.opp
fi

if [ "$(< config.hpp grep '#define COMPILE_JUMP' | grep -o '[0-9]')" == "1" ]; then
	echo "##### COMPILE $1 JUMP.CPP #####"
	$cc $debug $options $constants -c jump.cpp -o objects_"$1"/jump.opp
fi

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
$cc -m32 -shared -L/lib32 -o bin/lib$1.so -ldl $objects -lpthread $mysql_link $speex_link
rm objects_$1 -r

if [ mysql_variant > 0 ]; then
	sed -i "/#define COMPILE_MYSQL_DEFAULT 1/c\#define COMPILE_MYSQL_DEFAULT 0" config.hpp
	sed -i "/#define COMPILE_MYSQL_VORON 1/c\#define COMPILE_MYSQL_VORON 0" config.hpp
fi

# Read leftover
rm 0
