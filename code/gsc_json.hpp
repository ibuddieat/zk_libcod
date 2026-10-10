#ifndef _GSC_JSON_HPP_
#define _GSC_JSON_HPP_

#include "gsc.hpp"

void gsc_json_register_dvars(void);
void gsc_json_cleanup_on_level_load(void);
void gsc_json_shutdown(void);
void gsc_json_keep_field_names(void);

void gsc_json_parse();
void gsc_json_stringify();
void gsc_json_load();
void gsc_json_save();
void gsc_json_load_async();
void gsc_json_save_async();
void gsc_json_async_done();
void gsc_json_async_result();

#endif
