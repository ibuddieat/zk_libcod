#ifndef _GSC_UTILS_HPP_
#define _GSC_UTILS_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

/* link unlink */
#include <unistd.h>

void gsc_utils_printf();
void gsc_utils_getarraykeys();
void gsc_utils_getAscii();
void gsc_utils_toupper();
void gsc_utils_system();
void gsc_utils_exponent();
void gsc_utils_round();
void gsc_utils_file_link();
void gsc_utils_file_unlink();
void gsc_utils_file_exists();
void gsc_utils_FS_LoadDir();
void gsc_utils_getType();
void gsc_utils_float();
void gsc_utils_rundll();
void gsc_utils_ExecuteString();
void gsc_utils_sendgameservercommand();
void gsc_utils_scandir();
void gsc_G_FindConfigstringIndex();
void gsc_G_FindConfigstringIndexOriginal();
void gsc_get_configstring();
void gsc_set_configstring();
void gsc_make_localized_string();

void gsc_utils_fopen();
void gsc_utils_fread();
void gsc_utils_fwrite();
void gsc_utils_fclose();
void gsc_utils_fsize();
void gsc_utils_sprintf();
void gsc_utils_getsystemtime();
void gsc_utils_getlocaltime();

void gsc_call_function_raw();

void gsc_dlopen();
void gsc_dlsym();
void gsc_dlclose();

void gsc_utils_sqrt();
void gsc_utils_sqrtInv();

#ifdef __cplusplus
}
#endif

#endif
