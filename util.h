#pragma once

int exists(char* fn);
void allegro_message(char* m);
void set_config_file(char* f);
void set_config_string(char* prop, char* section, char* m);
void set_config_int(char* section, char* property, int v);
void set_config_float(char* section, char* property, float v);
float get_config_float(char* section, char* key, float defaultValue);
int get_config_int(char* section, char* key, int defaultValue);
char* get_config_string(char* section, char* key, char* defaultValue);