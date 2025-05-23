#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

// ALLOW UNSAFE SCRIPT FUNCTIONS
#define ENABLE_UNSAFE 0

// GSC MODULES
#define COMPILE_BOTS 1
#define COMPILE_ENTITY 1
#define COMPILE_LEVEL 1
#define COMPILE_MYSQL_DEFAULT 0
#define COMPILE_MYSQL_VORON 0
#define COMPILE_PLAYER 1
#define COMPILE_UTILS 1
#define COMPILE_WEAPONS 1
#if ENABLE_UNSAFE == 1
#define COMPILE_EXEC 1
#define COMPILE_MEMORY 1
#else
#define COMPILE_EXEC 0
#define COMPILE_MEMORY 0
#endif

// EXPERIMENTAL FEATURES
#define COMPILE_BSP 0
#define COMPILE_JUMP 1
#define COMPILE_CUSTOM_VOICE 1

#endif