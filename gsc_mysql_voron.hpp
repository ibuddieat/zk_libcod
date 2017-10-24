#ifndef _GSC_MYSQL_VORON_HPP_
#define _GSC_MYSQL_VORON_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* gsc functions */
#include "gsc.hpp"

void gsc_mysql_initialize();
void gsc_mysql_close();
void gsc_mysql_query();
void gsc_mysql_errno();
void gsc_mysql_error();
void gsc_mysql_affected_rows();
void gsc_mysql_store_result();
void gsc_mysql_num_rows();
void gsc_mysql_num_fields();
void gsc_mysql_field_seek();
void gsc_mysql_fetch_field();
void gsc_mysql_fetch_row();
void gsc_mysql_free_result();
void gsc_mysql_real_escape_string();

void gsc_async_mysql_initialize();
void gsc_async_mysql_close();
void gsc_async_mysql_create_query();
void gsc_async_mysql_create_query_nosave();
void gsc_async_mysql_checkdone();
void gsc_async_mysql_errno();
void gsc_async_mysql_error();
void gsc_async_mysql_affected_rows();
void gsc_async_mysql_num_rows();
void gsc_async_mysql_num_fields();
void gsc_async_mysql_field_seek();
void gsc_async_mysql_fetch_field();
void gsc_async_mysql_fetch_row();
void gsc_async_mysql_free_task();
void gsc_async_mysql_real_escape_string();

void gsc_async_mysql_create_entity_query(int entid);
void gsc_async_mysql_create_entity_query_nosave(int entid);

#ifdef __cplusplus
}
#endif

#endif