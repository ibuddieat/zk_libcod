#ifndef _GSC_EXEC_HPP_
#define _GSC_EXEC_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

/* offsetof */
#include <stddef.h>

void gsc_exec();
void gsc_exec_async_create();
void gsc_exec_async_create_nosave();
void gsc_exec_async_checkdone();

#ifdef __cplusplus
}
#endif

#endif