#ifndef _GSC_JSON_HPP_
#define _GSC_JSON_HPP_

#include "gsc.hpp"

void gsc_json_register_dvars(void);

void gsc_json_parse();
void gsc_json_stringify();
void gsc_json_load();
void gsc_json_save();
void gsc_json_load_async();
void gsc_json_save_async();
void gsc_json_async_done();
void gsc_json_async_result();

#endif
