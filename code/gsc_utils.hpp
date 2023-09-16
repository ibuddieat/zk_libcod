#ifndef _GSC_UTILS_HPP_
#define _GSC_UTILS_HPP_

/* gsc functions */
#include "gsc.hpp"

void Sys_AnsiColorPrint(const char *msg);

void gsc_utils_getsoundaliasesfromfile();
void gsc_utils_getsoundduration();

void gsc_utils_remotecommand();
void gsc_utils_executestring();
void gsc_utils_sendgameservercommand();
void gsc_utils_system();

void gsc_utils_printf();
void gsc_utils_sprintf();
void gsc_utils_outofbandprint();
void gsc_utils_putchar();
void gsc_utils_logprintconsole();

void gsc_utils_getarraykeys();
void gsc_utils_getascii();
void gsc_utils_toupper();

void gsc_utils_file_link();
void gsc_utils_file_unlink();
void gsc_utils_file_exists();
void gsc_utils_fs_loaddir();
void gsc_utils_scandir();
void gsc_utils_fopen();
void gsc_utils_fread();
void gsc_utils_fwrite();
void gsc_utils_fclose();
void gsc_utils_fsize();
void gsc_utils_fremove();

void gsc_utils_getsystemtime();
void gsc_utils_getserverstarttime();
void gsc_utils_getlocaltime();

void gsc_utils_g_findconfigstringindexoriginal();
void gsc_utils_g_findconfigstringindex();
void gsc_utils_get_configstring();
void gsc_utils_set_configstring();
void gsc_utils_make_localized_string();
void gsc_utils_make_client_localized_string();

void gsc_utils_float();
void gsc_utils_exponent();
void gsc_utils_round();
void gsc_utils_sqrt();
void gsc_utils_sqrtinv();
void gsc_utils_vectorscale();

void gsc_utils_chr();
void gsc_utils_ord();
void gsc_utils_fromhex();
void gsc_utils_tohex();

void gsc_utils_getlasttestclientnumber();
void gsc_utils_bullethiteffect();
void gsc_utils_gettype();

#if COMPILE_CUSTOM_VOICE == 1
void gsc_utils_getsoundfileduration();
void gsc_utils_loadsoundfile();
void gsc_utils_loadspeexfile();
void gsc_utils_savespeexfile();
#endif

#endif