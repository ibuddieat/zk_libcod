#ifndef _GSC_EXEC_HPP_
#define _GSC_EXEC_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* gsc functions */
#include "gsc.hpp"

void gsc_exec();
void gsc_exec_async_create();
void gsc_exec_async_create_nosave();
void gsc_exec_async_checkdone();

#ifdef __cplusplus
}
#endif

#endif