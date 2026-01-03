#include "stdafx.h"
#include <stdio.h>
#include "util.h"
#include <allegro5/allegro_native_dialog.h>

int exists(char* fn)
{
	FILE* t = fopen(fn, "rb");
	if (t)
	{
		fclose(t);
		return 1;
	}
	return 0;
}

void allegro_message(char* m)
{
	al_show_native_message_box(NULL, "Title", "Heading", m, NULL, ALLEGRO_MESSAGEBOX_WARN);
}

extern ALLEGRO_CONFIG* global_cfg;
void set_config_file(char* m)
{
	if (global_cfg) al_destroy_config(global_cfg);
	global_cfg = al_load_config_file(m);
}

void set_config_string(char* section, char* property, char* v)
{
	al_set_config_value(global_cfg, section, property, v);
}

void set_config_int(char* section, char* property, int v)
{
	char temp[80];
	sprintf(temp, "%d", v);
	al_set_config_value(global_cfg, section, property, temp);
}

void set_config_float(char* section, char* property, float v)
{
	char temp[80];
	sprintf(temp, "%f", v);
	al_set_config_value(global_cfg, section, property, temp);
}

float get_config_float (char* section, char* key, float defaultValue)
{
	if (!global_cfg) return defaultValue;
	char* value = (char*)al_get_config_value(global_cfg, section, key);
	if (value)
	{
		return atof(value);
	}
	else
	{
		return defaultValue;
	}
}

int get_config_int(char* section, char* key, int defaultValue)
{
	if (!global_cfg) return defaultValue;
	char* value = (char*)al_get_config_value(global_cfg, section, key);
	if (value)
	{
		return atoi(value);
	}
	else
	{
		return defaultValue;
	}
}

char* get_config_string(char* section, char* key, char* defaultValue)
{
	if (!global_cfg) return (char*)defaultValue;	
	char* value = (char*)al_get_config_value(global_cfg, section, key);
	if (value)
	{
		return value;
	}
	else
	{
		return defaultValue;
	}
	return NULL;
}