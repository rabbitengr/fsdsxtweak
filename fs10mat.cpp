#include "stdafx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include "util.h"

#include "fs10mat.h"
#include "EulerAngles.h"

extern ModelParams param_cache;
extern FS10Material material_cache[512];
extern int num_active_materials;
extern camera int_camera_cache[512];
extern camera ext_camera_cache[512];
extern int num_active_ext_cameras;
extern int num_active_int_cameras;

float cache_gearc[128];
contact gear_cache[128];
int gears_used = 0;
weight_station station_cache[128];
int stations_used = 0;
fs_exit exit_cache[128];
int exits_used = 0;
int ext_camera_count;
int int_camera_count;
float rad_to_deg(float rad)
{
	return rad*57.2957795;
}

void adjust_drp(float* x, float* y, float* z)
{
	return;

	*x -= param_cache.drp_x;
	*y -= param_cache.drp_y;
	*z -= param_cache.drp_z;
}

void read_ballast_parameters()
{
	char section_name[256];
	int k = 0;
	
	for (int k = 0; k < 128; k++)
	{
		sprintf(section_name, "ballast.%d", k);
				
		ballast_cache[k].capacity = get_config_float(section_name, "capacity", 10);
		ballast_cache[k].valve_index = get_config_int(section_name, "valve_index", 1);
		ballast_cache[k].x = get_config_float(section_name, "x", 0);
		ballast_cache[k].y = get_config_float(section_name, "y", 0);
		ballast_cache[k].z = get_config_float(section_name, "z", 0);
	}	
}

void read_smoke_parameters()
{
	char section_name[256];
	int k = 0;
	
	for (int k = 0; k < 128; k++)
	{
		sprintf(section_name, "smoke.%d", k);
		
		strcpy(smoke_cache[k].effect, get_config_string(section_name, "effect", "fx_smoke_w"));
		smoke_cache[k].x = get_config_float(section_name, "x", 0);
		smoke_cache[k].y = get_config_float(section_name, "y", 0);
		smoke_cache[k].z = get_config_float(section_name, "z", 0);
	}	
}

void read_fueltank_parameters()
{
	char* names[] = {
		"center1",
		"center2",
		"center3",
		"leftmain",
		"leftaux",
		"lefttip",
		"rightmain",
		"rightaux",
		"righttip",
		"external1",
		"external2"
	};
	
	char section_name[256];
	for (int k = 0; k < 11; k++)
	{
		sprintf(section_name, "fueltank.%s", names[k]);		
		tank_cache[k].usable = get_config_float(section_name, "usable", -1);
		tank_cache[k].unusable = get_config_float(section_name, "unusable", -1);
		tank_cache[k].x = get_config_float(section_name, "x", 0);
		tank_cache[k].y = get_config_float(section_name, "y", 0);
		tank_cache[k].z = get_config_float(section_name, "z", 0);

		if ((tank_cache[k].usable < 0) && (tank_cache[k].unusable < 0))
			tank_cache[k].inuse = 0;
		else
			tank_cache[k].inuse = 1;
	}	
}

void read_exit_parameters()
{
	char section_name[256];
	int k = 0;
	
	for (int k = 0; k < 128; k++)
	{
		sprintf(section_name, "exit.%d", k);
		
		exit_cache[k].speed = get_config_float(section_name, "speed", -1);
		exit_cache[k].type = get_config_int(section_name, "type", 0);
		exit_cache[k].x = get_config_float(section_name, "x", 0);
		exit_cache[k].y = get_config_float(section_name, "y", 0);
		exit_cache[k].z = get_config_float(section_name, "z", 0);
	}	
}


void read_station_parameters()
{
	char section_name[256];
	int k = 0;
	
	for (int k = 0; k < 128; k++)
	{
		sprintf(section_name, "stationload.%d", k);

		strcpy(station_cache[k].name, get_config_string(section_name, "name", ""));
		station_cache[k].weight = get_config_float(section_name, "weight", 0);
		station_cache[k].x = get_config_float(section_name, "x", 0);
		station_cache[k].y = get_config_float(section_name, "y", 0);
		station_cache[k].z = get_config_float(section_name, "z", 0);
	}
	
	for (int k = 0; k < 128; k++)
	{
		if (station_cache[k].name[0])
			stations_used = k+1;
	}		
}

void read_contact_parameters(contact* cache, char* sec)
{
	char section_name[256];
	int k = 0;
	
	for (int k = 0; k < 128; k++)
	{
		sprintf(section_name, "%s.%d", sec, k);
		

		cache[k].airspeed_limit = get_config_float(section_name, "airspeed_limit", 0);
		cache[k].brake_map = get_config_int(section_name, "brake_map", 0);
		cache[k].Class = get_config_int(section_name, "class", 0);
		cache[k].damage_airspeed = get_config_float(section_name, "airspeed_damage", 0);
		cache[k].damping_ratio = get_config_float(section_name, "damping_ratio", 0);
		cache[k].extension_time = get_config_float(section_name, "extension_time", 0);
		cache[k].impact_damage_threshold = get_config_float(section_name, "impact_damage_threshold", 0);
		cache[k].max_static_compression_ratio = get_config_float(section_name, "max_static_compression_ratio", 0);
		cache[k].retraction_time = get_config_float(section_name, "retraction_time", 0);
		cache[k].sound_type = get_config_int(section_name, "sound_type", 0);
		cache[k].static_compression = get_config_float(section_name, "static_compression", 0);
		cache[k].steering_angle = get_config_float(section_name, "steering_angle", 0);
		cache[k].wheel_radius = get_config_float(section_name, "wheel_radius", 0);
		cache[k].x = get_config_float(section_name, "x", 0);
		cache[k].y = get_config_float(section_name, "y", 0);
		cache[k].z = get_config_float(section_name, "z", 0);
	}
}

void read_globalcontact()
{
	//read the global contact stuff 
	globalcontact.process = 0;

	globalcontact.emergency_extension_type = get_config_int("contact_global", "emegency_extension_type", -1);
	globalcontact.gear_system_type = get_config_int("contact_global", "gear_system_type", -1);
	globalcontact.max_number_of_points = get_config_int("contact_global", "max_number_of_points", -1);
	globalcontact.tailwheel_lock = get_config_int("contact_global", "tailwheel_lock", -1);
	globalcontact.static_cg_height = get_config_float("contact_global", "static_cg_height", -1);
	globalcontact.static_pitch = get_config_float("contact_global", "static_pitch", -1);

	if (globalcontact.emergency_extension_type != -1)
		globalcontact.process = 1;
	if (globalcontact.gear_system_type != -1)
		globalcontact.process = 1;
	if (globalcontact.max_number_of_points != -1)
		globalcontact.process = 1;
	if (globalcontact.tailwheel_lock != -1)
		globalcontact.process = 1;
	if (globalcontact.static_cg_height != -1)
		globalcontact.process = 1;
	if (globalcontact.static_pitch != -1)
		globalcontact.process = 1;
}

void validate_AlphaTestFunction(int k)
{
	material_cache[k].Alpha_AlphaTestFunctionFLAG = 0;

	if (!strcmp(material_cache[k].Alpha_AlphaTestFunction, "Never"))
	{
		material_cache[k].Alpha_AlphaTestFunctionFLAG = 1;
	}

	if (!strcmp(material_cache[k].Alpha_AlphaTestFunction, "Less"))
	{
		material_cache[k].Alpha_AlphaTestFunctionFLAG = 1;
	}

	if (!strcmp(material_cache[k].Alpha_AlphaTestFunction, "Equal"))
	{
		material_cache[k].Alpha_AlphaTestFunctionFLAG = 1;
	}

	if (!strcmp(material_cache[k].Alpha_AlphaTestFunction, "LessEqual"))
	{
		material_cache[k].Alpha_AlphaTestFunctionFLAG = 1;
	}

	if (!strcmp(material_cache[k].Alpha_AlphaTestFunction, "Greater"))
	{
		material_cache[k].Alpha_AlphaTestFunctionFLAG = 1;
	}

	if (!strcmp(material_cache[k].Alpha_AlphaTestFunction, "NotEqual"))
	{
		material_cache[k].Alpha_AlphaTestFunctionFLAG = 1;
	}

	if (!strcmp(material_cache[k].Alpha_AlphaTestFunction, "GreaterEqual"))
	{
		material_cache[k].Alpha_AlphaTestFunctionFLAG = 1;
	}

	if (!strcmp(material_cache[k].Alpha_AlphaTestFunction, "Always"))
	{
		material_cache[k].Alpha_AlphaTestFunctionFLAG = 1;
	}
}

void read_texture_parameters()
{
	char section_name[256];
	int k = 0;
	
	//read the model params
	sprintf(section_name, "model");
	param_cache.ShadowMapReady = get_config_int(section_name, "ShadowMapReady", 0);
	param_cache.UnitsMetric = get_config_int(section_name, "UnitsMetric", 0);
		
	//read the materials
	while(1)
	{
		sprintf(section_name, "texture.%d", k);
		strcpy(material_cache[k].identifier, get_config_string(section_name, "identifier", ""));
		if (material_cache[k].identifier[0] == '\0')
		{
			break;
		}

		//maps
		strcpy(material_cache[k].diffuse, get_config_string(section_name, "diffuse", ""));		
		strcpy(material_cache[k].specular, get_config_string(section_name, "specular", ""));
		strcpy(material_cache[k].ambient, get_config_string(section_name, "ambient", ""));
		strcpy(material_cache[k].emissive, get_config_string(section_name, "emissive", ""));
		strcpy(material_cache[k].reflection, get_config_string(section_name, "reflection", ""));
		strcpy(material_cache[k].shininess, get_config_string(section_name, "shininess", ""));
		strcpy(material_cache[k].bump, get_config_string(section_name, "bump", ""));
		strcpy(material_cache[k].displacement, get_config_string(section_name, "displacement", ""));
		strcpy(material_cache[k].detail, get_config_string(section_name, "detail", ""));
		strcpy(material_cache[k].fresnel, get_config_string(section_name, "fresnel", ""));
	
		//enhanced parameters;
		material_cache[k].ep_assume_vertical_normal = get_config_int(section_name, "AssumeVerticalNormal", 0);
		material_cache[k].ep_no_shadow = get_config_int(section_name, "NoShadow", 0);
		material_cache[k].ep_no_z_write = get_config_int(section_name, "NoZWrite", 0);
		material_cache[k].ep_prelit_vertices = get_config_int(section_name, "PrelitVertices", 0);
		material_cache[k].ep_volume_shadow = get_config_int(section_name, "VolumeShadow", 0);
		material_cache[k].ep_z_write_alpha = get_config_int(section_name, "ZWriteAlpha", 0);			

		
		material_cache[k].EnvironmentLevelScale = get_config_float(section_name, "environment_level_scale", -1);
		material_cache[k].detail_scale = get_config_float(section_name, "detail_scale", -1);
		material_cache[k].bump_scale = get_config_float(section_name, "bump_scale", -1);

		//material props
		//mp_use_global_env
		//mp_blend_env_by_inv_diff_alpha
		//mp_blend_env_by_specular_alpha
		//mp_fresnel_diffuse
		//mp_fresnel_specular
		//mp_fresnel_env
		//mp_precip_use
		//mp_precip_offset
		//mp_precip_offset_value
		//mp_specular_map_power_scale

		material_cache[k].mp_use_global_env = get_config_int(section_name, "UseGlobalEnvironment", -1);
		material_cache[k].mp_blend_env_by_inv_diff_alpha = get_config_int(section_name, "BlendEnvironmentByInverseDiffuseAlpha", -1);
		material_cache[k].mp_blend_env_by_specular_alpha = get_config_int(section_name, "BlendEnvironmentBySpecularAlpha", -1);
		material_cache[k].mp_fresnel_diffuse = get_config_int(section_name, "FresnelDiffuse", -1);
		material_cache[k].mp_fresnel_specular = get_config_int(section_name, "FresnelSpecular", -1);
		material_cache[k].mp_fresnel_env = get_config_int(section_name, "FresnelEnvironment", -1);
		material_cache[k].mp_precip_use = get_config_int(section_name, "PrecipitationUse", -1);
		material_cache[k].mp_precip_offset = get_config_int(section_name, "PrecipitationOffset", -1);		
		material_cache[k].mp_precip_offset_value = get_config_float(section_name, "PrecipitationOffsetValue", -1);
		material_cache[k].mp_specular_map_power_scale = get_config_float(section_name, "SpecularMapPowerScale", -1);

		strcpy(material_cache[k].SrcBlend, get_config_string(section_name, "SourceBlend", "NULL"));
		adjust_src_dst_strings(material_cache[k].SrcBlend, 1, &material_cache[k].SrcBlendFLAG,  &material_cache[k].DestBlendFLAG);
		strcpy(material_cache[k].DestBlend, get_config_string(section_name, "DestinationBlend", "NULL"));
		adjust_src_dst_strings(material_cache[k].DestBlend, 0, &material_cache[k].SrcBlendFLAG,  &material_cache[k].DestBlendFLAG);

		strcpy(material_cache[k].EmissiveMode, get_config_string(section_name, "EmissiveMode", "NULL"));
		adjust_emissive_strings(material_cache[k].EmissiveMode, &material_cache[k].EmissiveModeFLAG);
		
		material_cache[k].misc_doublesided = get_config_int(section_name, "DoubleSided", 0);
		material_cache[k].misc_isnnumber = get_config_int(section_name, "IsNNumber", 0);

		material_cache[k].bloom_allowbloom = get_config_int(section_name, "AllowBloom", -1);
		material_cache[k].bloom_ambientlightscale = get_config_float(section_name, "AmbientLightScale", -1);
		material_cache[k].bloom_bloombycopy = get_config_int(section_name, "BloomByCopy", 0);
		material_cache[k].bloom_bloommodulatingbyalpha = get_config_int(section_name, "BloomModulatingByAlpha", 0);
		material_cache[k].bloom_emissivebloom = get_config_int(section_name, "EmissiveBloom", 0);
		material_cache[k].bloom_nospecularbloom = get_config_int(section_name, "NoSpecularBloom", 0);
		material_cache[k].bloom_specularbloomfloor = get_config_float(section_name, "SpecularBloomFloor", -1);
				
		material_cache[k].diffuse_a = get_config_float(section_name, "diffuse_a", -1);
		material_cache[k].diffuse_r = get_config_float(section_name, "diffuse_r", -1);
		material_cache[k].diffuse_g = get_config_float(section_name, "diffuse_g", -1);
		material_cache[k].diffuse_b = get_config_float(section_name, "diffuse_b", -1);
		material_cache[k].diffuse_ok = 1;
		if (
			(material_cache[k].diffuse_a == -1)||
			(material_cache[k].diffuse_r == -1)||
			(material_cache[k].diffuse_g == -1)||
			(material_cache[k].diffuse_b == -1)
			)
		{
			material_cache[k].diffuse_ok = 0;	
		}

		material_cache[k].specular_r = get_config_float(section_name, "specular_r", -1);
		material_cache[k].specular_g = get_config_float(section_name, "specular_g", -1);
		material_cache[k].specular_b = get_config_float(section_name, "specular_b", -1);
		material_cache[k].specular_p = get_config_float(section_name, "specular_power", -1);
		material_cache[k].specular_ok = 1;
		if (			
			(material_cache[k].specular_r == -1)||
			(material_cache[k].specular_g == -1)||
			(material_cache[k].specular_b == -1)			 
			)
		{
			material_cache[k].specular_ok = 0;	
		}

		//v2.8 stuff
		//bool BlendDiffuseByInverseSpecularMapAlpha
		//bool BlendDiffuseByBaseAlpha
		//bool ForceTextureAddressWrapSetting
		//bool ForceTextureAddressClamp
		//AlphaData
		//bool	ZTestAlpha
		//float	AlphaTestValue
		//string	AlphaTestFunction
		//bool	FinalAlphaWrite
		//float	FinalAlphaWriteValue
		material_cache[k].BlendDiffuseByInverseSpecularMapAlpha = get_config_int(section_name, "BlendDiffuseByInverseSpecularMapAlpha", 0);
		material_cache[k].BlendDiffuseByBaseAlpha = get_config_int(section_name, "BlendDiffuseByBaseAlpha", 0);
		material_cache[k].ForceTextureAddressWrapSetting = get_config_int(section_name, "ForceTextureAddressWrapSetting", 0);
		material_cache[k].ForceTextureAddressClamp = get_config_int(section_name, "ForceTextureAddressClamp", 0);
		material_cache[k].Alpha_ZTestAlpha = get_config_int(section_name, "ZTestAlpha", 0);
		material_cache[k].Alpha_AlphaTestValue = get_config_float(section_name, "AlphaTestValue", -1);		
		strcpy(material_cache[k].Alpha_AlphaTestFunction, get_config_string(section_name, "AlphaTestFunction", ""));
		validate_AlphaTestFunction(k);
		material_cache[k].Alpha_FinalAlphaWrite = get_config_int(section_name, "FinalAlphaWrite", 0);
		material_cache[k].Alpha_FinalAlphaWriteValue = get_config_float(section_name, "FinalAlphaWriteValue", -1);		

		k++;
		num_active_materials++;
	}

	printf("MATERIAL CONFIG: Read %d materials to change\n", num_active_materials);
	for (int i = 0; i < num_active_materials; i++)
	{
		printf("\tid: '%s' ", material_cache[i].identifier);
		if (material_cache[i].diffuse[0])
			printf("Diffuse ");
		if (material_cache[i].specular[0])
			printf("Specular ");
		if (material_cache[i].ambient[0])
			printf("Ambient ");
		if (material_cache[i].emissive[0])
			printf("Emissive ");
		if (material_cache[i].reflection[0])
			printf("Reflection ");
		if (material_cache[i].shininess[0])
			printf("Shininess ");
		if (material_cache[i].bump[0])
			printf("Bump ");
		if (material_cache[i].displacement[0])
			printf("Displacement ");
		if (material_cache[i].fresnel[0])
			printf("Fresnel ");
		printf("\n");
		printf("\t  Enhanced properties: ");
		printf("%d ", material_cache[i].ep_assume_vertical_normal?1:0);
		printf("%d ", material_cache[i].ep_z_write_alpha?1:0);
		printf("%d ", material_cache[i].ep_no_z_write?1:0);
		printf("%d ", material_cache[i].ep_volume_shadow?1:0);
		printf("%d ", material_cache[i].ep_no_shadow?1:0);
		printf("%d ", material_cache[i].ep_prelit_vertices?1:0);
		printf("\n");
			
	}
	
}

///////////////////////////////////////////////////////////
//
//		MODEL TWEAKS
//
///////////////////////////////////////////////////////////

int read_model_parameters()
{
	//These are all tweaks which are applied to the aircraft.cfg file!

	//returns 0 if no model tweaks are to be made
	return 1;
}

void process_secondary_rotor(FILE* in)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	adjust_drp(&x, &y, &z);

	printf("Processing secondary rotor at {x,y,z} = (%f %f %f)\n", x, y, z);
	
	char setstring[1024];
		
	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}
		
	sprintf(setstring, "%f, %f, %f", z, x, -y);
	set_config_string("secondaryrotor", "position", setstring);
}

void process_main_rotor(FILE* in)
{
	//get the angles and pos
	char buff[1024];
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, "FrameTransformMatrix"));

	float mat[4][4];

	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][0]), &(mat[1][0]), &(mat[2][0]),&(mat[3][0]));
	
	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][1]), &(mat[1][1]), &(mat[2][1]),&(mat[3][1]));
	
	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][2]), &(mat[1][2]), &(mat[2][2]),&(mat[3][2]));
	
	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][3]), &(mat[1][3]), &(mat[2][3]),&(mat[3][3]));

	if (param_cache.UnitsMetric)
	{
		mat[0][3] *= FEET_TO_METER;
		mat[1][3] *= FEET_TO_METER;
		mat[2][3] *= FEET_TO_METER;
	}

	printf("Processing main rotor at {x,y,z} = (%f %f %f)\n", mat[0][3], mat[1][3], mat[2][3]);
	char pos[512];
	sprintf(pos, "%f, %f, %f", mat[0][3], -mat[2][3], mat[1][3]);	

	set_config_string("mainrotor", "position", pos);
	EulerAngles e = Eul_FromHMatrix(mat, EulOrdXYZs);

	float p = rad_to_deg(e.x);
	float b = rad_to_deg(e.y);
	float h = -rad_to_deg(e.z);

	char rot[512];
	sprintf(rot, "%f", p);
	set_config_string("mainrotor", "static_pitch_angle", rot);
	sprintf(rot, "%f", b);
	set_config_string("mainrotor", "static_bank_angle", rot);	
}

void process_anemometers(FILE* in, int anemometer_offset)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	adjust_drp(&x, &y, &z);

	printf("Processing anemometer %d at {x,y,z} = (%f %f %f)\n", anemometer_offset, x, y, z);
	char section[80];
	char setstring[1024];
	sprintf(section, "anemometer.%d", anemometer_offset);
	
	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}
		
	sprintf(setstring, "%f, %f, %f", z, x, -y);
	set_config_string("anemometers", section, setstring);
}

void process_gearu(FILE* in, int offset)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	adjust_drp(&x, &y, &z);

	printf("Processing uncompressed gear %d at {x,y,z} = (%f %f %f)\n", offset, x, y, z);
	char section[80];
	char setstring[1024];
	
	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}
	
	gear_cache[offset].x = x;
	gear_cache[offset].y = -y;
	gear_cache[offset].z = z;
	gear_cache[offset].used = 1;
	
	gears_used++;
}

void process_gearc(FILE* in, int offset)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	adjust_drp(&x, &y, &z);

	printf("Processing compressed gear %d at {x,y,z} = (%f %f %f)\n", offset, x, y, z);
	char section[80];
	char setstring[1024];
		
	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}
		
	cache_gearc[offset] = -y;	
}


void process_ballast(FILE* in, int ballast_offset)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	adjust_drp(&x, &y, &z);

	printf("Processing ballast tank %d at {x,y,z} = (%f %f %f)\n", ballast_offset, x, y, z);
	char section[80];
	char setstring[1024];
	sprintf(section, "tank.%d", ballast_offset);
	
	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}
	
	if (ballast_cache[ballast_offset].x)
		x = ballast_cache[ballast_offset].x;
	if (ballast_cache[ballast_offset].y)
		y = ballast_cache[ballast_offset].y;
	if (ballast_cache[ballast_offset].z)
		z = ballast_cache[ballast_offset].z;

	sprintf(setstring, "%f, %f, %f, %f, %d", ballast_cache[ballast_offset].capacity, z, x, -y, ballast_cache[ballast_offset].valve_index);
	set_config_string("waterballastsystem", section, setstring);
}


void process_smoke(FILE* in, int smoke_offset)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	adjust_drp(&x, &y, &z);

	printf("Processing smoke %d at {x,y,z} = (%f %f %f)\n", smoke_offset, x, y, z);
	char section[80];
	char setstring[1024];
	sprintf(section, "smoke.%d", smoke_offset);
	
	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}
		
	if (smoke_cache[smoke_offset].x)
		x = smoke_cache[smoke_offset].x;
	if (smoke_cache[smoke_offset].y)
		y = smoke_cache[smoke_offset].y;
	if (smoke_cache[smoke_offset].z)
		z = smoke_cache[smoke_offset].z;

	sprintf(setstring, "%f, %f, %f, %s", z, x, -y, smoke_cache[smoke_offset].effect);
	set_config_string("smokesystem", section, setstring);
}

void process_fuel_tank(int offset, FILE* in, char* tankname, float usecap, float unusecap)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	printf("Processing fuel tank '%s' at {x,y,z} (%f, %f) gallons = (%f %f %f)\n", tankname, usecap, unusecap, x, y, z);

	char section[80];
	char setstring[1024];
	
	adjust_drp(&x, &y, &z);

	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}
	
	if (tank_cache[offset].x)
		x = tank_cache[offset].x;
	if (tank_cache[offset].y)
		y = tank_cache[offset].y;
	if (tank_cache[offset].z)
		z = tank_cache[offset].z;

	sprintf(setstring, "%f, %f, %f, %f, %f", z, x, -y, usecap, unusecap);
	set_config_string("fuel", tankname, setstring);
}

void process_exits(FILE* in, int exit_offset)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	adjust_drp(&x, &y, &z);

	printf("Processing exit %d at {x,y,z} = (%f %f %f)\n", exit_offset, x, y, z);
	char section[80];
	char setstring[1024];
	sprintf(section, "exit.%d", exit_offset);
	
	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}
	
	if (exit_cache[exit_offset].x)
		x = exit_cache[exit_offset].x;
	if (exit_cache[exit_offset].y)
		y = exit_cache[exit_offset].y;
	if (exit_cache[exit_offset].z)
		z = exit_cache[exit_offset].z;

	char name[1024];
	float speed = 3;	
	if (exit_cache[exit_offset].speed > 0)
	{
		 speed = exit_cache[exit_offset].speed;
	}

	sprintf(setstring, "%f, %f, %f, %f, %d", speed, z, x, -y, exit_cache[exit_offset].type);
	set_config_string("exits", section, setstring);
	
	int e = exit_offset +1;
	if (e > exits_used)
		exits_used = e;
}


void process_stations(FILE* in, int station_offset)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	adjust_drp(&x, &y, &z);

	printf("Processing weight station %d at {x,y,z} = (%f %f %f)\n", station_offset, x, y, z);
	char section[80];
	char setstring[1024];
	sprintf(section, "station_load.%d", station_offset);
	
	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}
	
	char name[1024];
	float weight = 10;
	if (station_cache[station_offset].weight > 0)
	{
		weight = station_cache[station_offset].weight;
	}
	if (station_cache[station_offset].name[0])
	{
		strcpy(name, station_cache[station_offset].name);
	}
	else
	{
		sprintf(name, "Weight Station %d", station_offset);
	}

	if (station_cache[station_offset].x)
		x = station_cache[station_offset].x;
	if (station_cache[station_offset].y)
		y = station_cache[station_offset].y;
	if (station_cache[station_offset].z)
		z = station_cache[station_offset].z;

	sprintf(setstring, "%f, %f, %f, %f, %s", weight, z, x, -y, name);
	set_config_string("weight_and_balance", section, setstring);
}

void process_lift_aero_center(FILE* in)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	printf("Processing lift aero center at %f\n", z);

	char section[80];
	char setstring[1024];
	
	if (param_cache.UnitsMetric)
	{
		z *= FEET_TO_METER;
	}
	
	set_config_float("helicopter", "lift_aero_center", z);
}


void process_empty_cg(FILE* in)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	printf("Processing empty CG at %f\n", z);

	char section[80];
	char setstring[1024];
	
	if (param_cache.UnitsMetric)
	{
		z *= FEET_TO_METER;
	}
	
	set_config_float("weight_and_balance", "empty_weight_CG_position", z);
}


void process_datum_reference_point(FILE* in)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	printf("Processing Datum RP at {x,y,z} = (%f %f %f)\n", x, y, z);

	char section[80];
	char setstring[1024];
	
	param_cache.drp_x = x;
	param_cache.drp_y = y;
	param_cache.drp_z = z;

	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}

	sprintf(setstring, "%f, %f, %f", z, x, y);
	set_config_string("weight_and_balance", "reference_datum_position", setstring);
}

void process_pilot_view(FILE* in)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	printf("Processing eyepoint at {x,y,z} = (%f %f %f)\n", x, y, z);

	char section[80];
	char setstring[1024];
	
	adjust_drp(&x, &y, &z);

	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}

	sprintf(setstring, "%f, %f, %f", z, x, -y);
	set_config_string("Views", "eyepoint", setstring);
}

void process_engine(FILE* in, int engine_offset)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	adjust_drp(&x, &y, &z);

	printf("Processing engine %d at {x,y,z} = (%f %f %f)\n", engine_offset, x, y, z);
	char section[80];
	char setstring[1024];
	sprintf(section, "Engine.%d", engine_offset);
	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}
	sprintf(setstring, "%f, %f, %f", z, x, -y);
	set_config_string("GeneralEngineData", section, setstring);
}

void process_globalcontact()
{
	if (!globalcontact.process)
		return;

	printf("Processing global contact information\n");

	if (globalcontact.emergency_extension_type != -1)
	{
		set_config_int("contact_points", "emergency_extension_type", globalcontact.emergency_extension_type);
	}
	if (globalcontact.gear_system_type != -1)
	{
		set_config_int("contact_points", "gear_system_type", globalcontact.gear_system_type);
	}
	if (globalcontact.max_number_of_points != -1)
	{
		set_config_int("contact_points", "max_number_of_points", globalcontact.max_number_of_points);
	}
	if (globalcontact.tailwheel_lock != -1)
	{
		set_config_int("contact_points", "tailwheel_lock", globalcontact.tailwheel_lock);
	}
	if (globalcontact.static_cg_height != -1)
	{
		set_config_int("contact_points", "static_cg_height", globalcontact.static_cg_height);
	}
	if (globalcontact.static_pitch != -1)
	{
		set_config_int("contact_points", "static_pitch", globalcontact.static_pitch);
	}
}

void process_contact(FILE* in, int contact_offset, int _type)
{
	//look for the ;;
	char buff[1024];
	buff[0] = '\0';
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, ";;"));

	//now read in the position from the current line
	float x, y, z, one;
	sscanf(buff, "%f, %f, %f, %f", &x, &z, &y, &one);
	
	adjust_drp(&x, &y, &z);

	printf("Processing contact point %d at {x,y,z} = (%f %f %f)\n", contact_offset, x, y, z);
	char section[80];
	char setstring[1024];
	sprintf(section, "point.%d", contact_offset);
	if (param_cache.UnitsMetric)
	{
		x *= FEET_TO_METER;
		y *= FEET_TO_METER;
		z *= FEET_TO_METER;
	}
	
	int type = _type;
	if (contact_cache[contact_offset].Class)
	{
		type = contact_cache[contact_offset].Class;
	}

	if (contact_cache[contact_offset].x != 0)
	{
		x = contact_cache[contact_offset].x;
	}
	if (contact_cache[contact_offset].y != 0)
	{
		y = contact_cache[contact_offset].y;
	}
	if (contact_cache[contact_offset].z != 0)
	{
		z = contact_cache[contact_offset].z;
	}
	sprintf(setstring, "%d, %f, %f, %f, %f, %d, %f, %f, %f, %f, %f, %f, %f, %d, %f, %f", 
			type,	
			z, 
			x, 
			-y,
			contact_cache[contact_offset].impact_damage_threshold,	
			contact_cache[contact_offset].brake_map,
			contact_cache[contact_offset].wheel_radius,
			contact_cache[contact_offset].steering_angle,
			contact_cache[contact_offset].static_compression,
			contact_cache[contact_offset].max_static_compression_ratio,
			contact_cache[contact_offset].damping_ratio,
			contact_cache[contact_offset].extension_time,
			contact_cache[contact_offset].retraction_time,
			contact_cache[contact_offset].sound_type,
			contact_cache[contact_offset].airspeed_limit,
			contact_cache[contact_offset].damage_airspeed);
	
	set_config_string("contact_points", section, setstring);

	contacts_used++;
}



void extract_name(char* in, char* out)
{
	int pos = 0;
	int l = strlen(in);
	for (int k = 0; k < l; k++)
	{
		if (in[k] == '#')
		{
			pos = k+1;
			break;
		}
	}
	if (pos == 0)
	{
		sprintf(out, "No name");
		return;
	}
	int c = 1;
	out[0] = '"';
	for (int k = pos; k < l; k++)
	{
		if (in[k] == '#')
			break;
		if (in[k] == '\0')
			break;

		out[c] = in[k];
		c++;
	}

	out[c] = '"';
	out[c+1] = '\0';
}

int extract_number(char* line)
{
	int pos = strlen(line);
	while(pos >= 0)
	{
		if (line[pos] == '_')
			break;
		pos--;
	}
	if (pos == 0)
		return 0;

	return (atoi(line+pos+1));
}

void process_ext_cam(char* line, FILE* in, int count)
{
	//get the name of the cam from the line
	char cam_name[1024];
	int my_number = extract_number(line);	
		
	printf("Processing external camera %d ", my_number);
	//do we have adjustments?
	int offset = -1;
	for (int k = 0; k < 512; k++)
	{
		if (ext_camera_cache[k].identifier == my_number)
			offset = my_number;
	}

	if (offset > -1)
	{
		printf("(adjustments defined)\n");
	}
	else
		printf("(no adjustments defined)\n");


	//get the angles and pos
	char buff[1024];
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, "FrameTransformMatrix"));

	float mat[4][4];

	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][0]), &(mat[1][0]), &(mat[2][0]),&(mat[3][0]));
	
	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][1]), &(mat[1][1]), &(mat[2][1]),&(mat[3][1]));
	
	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][2]), &(mat[1][2]), &(mat[2][2]),&(mat[3][2]));
	
	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][3]), &(mat[1][3]), &(mat[2][3]),&(mat[3][3]));

	//write the camera out
	char section[80];
	char guid[1024];
	char temp[80];
	build_guid(guid);
	sprintf(section, "CameraDefinition.%d", my_number);	
	if (ext_camera_cache[my_number].title[0])
	{
		set_config_string(section, "Title", ext_camera_cache[my_number].title);
	}
	else
	{
		sprintf(temp, "FSDS External Camera %d", my_number);
		set_config_string(section, "Title", temp);
	}
	if (ext_camera_cache[my_number].guid_touched)
		set_config_string(section, "Guid", ext_camera_cache[my_number].guid);
	else
		set_config_string(section, "Guid", guid);

	if (ext_camera_cache[my_number].origin_touched)
		set_config_string(section, "Origin", ext_camera_cache[my_number].origin);
	else
		set_config_string(section, "Origin", "Center");

	if (ext_camera_cache[my_number].category_touched)
		set_config_string(section, "category", ext_camera_cache[my_number].category);
	else
		set_config_string(section, "category", "Aircraft");

	if (ext_camera_cache[my_number].track_touched)
		set_config_string(section, "track", ext_camera_cache[my_number].track);
	else
		set_config_string(section, "track", "none");

	if (ext_camera_cache[my_number].snappbhadjust_touched)
		set_config_string(section, "SnappbhAdjust", ext_camera_cache[my_number].snappbhadjust);
	else
		set_config_string(section, "SnappbhAdjust", "swivel");

	char pos[80];
	char rot[80];
		
	if (offset > -1)
	{
		if (ext_camera_cache[offset].InitialXyz_x_touched)
			mat[0][3] = ext_camera_cache[offset].InitialXyz_x;
		if (ext_camera_cache[offset].InitialXyz_y_touched)
			mat[1][3] = ext_camera_cache[offset].InitialXyz_y;
		if (ext_camera_cache[offset].InitialXyz_z_touched)
			mat[2][3] = ext_camera_cache[offset].InitialXyz_z;
	}

	sprintf(pos, "%f, %f, %f", mat[0][3], -mat[2][3], mat[1][3]);	

	set_config_string(section, "InitialXyz", pos);
	EulerAngles e = Eul_FromHMatrix(mat, EulOrdXYZs);

	float p = rad_to_deg(e.x);
	float b = rad_to_deg(e.y);
	float h = -rad_to_deg(e.z);

	if (offset > -1)
	{
		if (ext_camera_cache[offset].InitialPbh_p_touched)
			p = ext_camera_cache[offset].InitialPbh_p;
		if (ext_camera_cache[offset].InitialPbh_p_touched)
			b = ext_camera_cache[offset].InitialPbh_b;
		if (ext_camera_cache[offset].InitialPbh_p_touched)
			h = ext_camera_cache[offset].InitialPbh_h;
	}

	sprintf(rot, "%f, %f, %f", p,b,h);
	set_config_string(section, "InitialPbh", rot);

	if (offset == -1)
		return;
	else
		do_camera_other_adjustments(ext_camera_cache, section, offset);
}

void get_eyepoint(char* cfg)
{	
	set_config_file(cfg);
	char buff[1024];
	strcpy(buff, get_config_string("[Views]", "eyepoint", "0,0,0"));	
	sscanf(buff, "%f, %f, %f", &param_cache.eyepoint_x, &param_cache.eyepoint_y, &param_cache.eyepoint_z);	
	param_cache.eyepoint_metric_x = param_cache.eyepoint_x / FEET_TO_METER;
	param_cache.eyepoint_metric_y = param_cache.eyepoint_y / FEET_TO_METER;
	param_cache.eyepoint_metric_z = param_cache.eyepoint_z / FEET_TO_METER;
}

void process_int_cam(char* line, FILE* in, int count)
{
	//get the name of the cam from the line
	char cam_name[1024];
	int my_number = extract_number(line);	
	printf("my number %d\n", my_number);
		
	printf("Processing virtual cockpit camera %d ", my_number);
	//do we have adjustments?
	int offset = -1;	
	for (int k = 0; k < 512; k++)
	{
		if (int_camera_cache[k].identifier == my_number)
			offset = my_number;
	}
	
	if (offset > -1)
	{
		printf("(adjustments defined)\n");
	}
	else
		printf("(no adjustments defined)\n");

	
	//get the angles and pos
	char buff[1024];
	do
	{
		fgets(buff, 1024, in);
	}while(!strstr(buff, "FrameTransformMatrix"));

	float mat[4][4];

	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][0]), &(mat[1][0]), &(mat[2][0]),&(mat[3][0]));
	
	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][1]), &(mat[1][1]), &(mat[2][1]),&(mat[3][1]));
	
	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][2]), &(mat[1][2]), &(mat[2][2]),&(mat[3][2]));
	
	fgets(buff, 1024, in);
	sscanf(buff, "%f, %f, %f, %f,", &(mat[0][3]), &(mat[1][3]), &(mat[2][3]),&(mat[3][3]));

	//write the camera out
	char section[80];
	char guid[1024];
	char temp[80];
	build_guid(guid);
	sprintf(section, "CameraDefinition.%03d", my_number+1);		

	if (int_camera_cache[my_number].title_touched)
	{
		set_config_string(section, "Title", int_camera_cache[my_number].title);
	}
	else
	{
		sprintf(temp, "FSDS VC Camera %03d", my_number+1);
		set_config_string(section, "Title", temp);
	}

	if (int_camera_cache[my_number].guid_touched)
		set_config_string(section, "Guid", int_camera_cache[my_number].guid);
	else
		set_config_string(section, "Guid", guid);
	
	if (int_camera_cache[my_number].origin_touched)
		set_config_string(section, "Origin", int_camera_cache[my_number].origin);
	else
		set_config_string(section, "Origin", "Virtual Cockpit");
	
	if (int_camera_cache[my_number].category_touched)
		set_config_string(section, "category", int_camera_cache[my_number].category);
	else
		set_config_string(section, "category", "Cockpit");

	if (int_camera_cache[my_number].track_touched)
		set_config_string(section, "track", int_camera_cache[my_number].track);
	else
		set_config_string(section, "track", "none");
	
	if (int_camera_cache[my_number].snappbhadjust_touched)
		set_config_string(section, "SnappbhAdjust", int_camera_cache[my_number].snappbhadjust);
	else
		set_config_string(section, "SnappbhAdjust", "swivel");

	char pos[80];
	char rot[80];

	//subtract the eye pos from this
	float x, y, z;
	float source_x = mat[0][3];
	float source_y = -mat[2][3];
	float source_z = mat[1][3];

	z = -param_cache.eyepoint_metric_x + source_z;
	y = param_cache.eyepoint_metric_y + source_y;
	x = param_cache.eyepoint_metric_z + source_x;

	if (offset > -1)
	{
		if (int_camera_cache[offset].InitialXyz_x_touched)
			x = int_camera_cache[offset].InitialXyz_x;
		if (int_camera_cache[offset].InitialXyz_y_touched)
			y = int_camera_cache[offset].InitialXyz_y;
		if (int_camera_cache[offset].InitialXyz_z_touched)
			z = int_camera_cache[offset].InitialXyz_z;
	}
	
	sprintf(pos, "%f, %f, %f", x, y, z);	

	set_config_string(section, "InitialXyz", pos);
	EulerAngles e = Eul_FromHMatrix(mat, EulOrdXYZs);

	float p = rad_to_deg(e.x);
	float b = rad_to_deg(e.y);
	float h = -rad_to_deg(e.z);

	if (offset > -1)
	{
		if (int_camera_cache[offset].InitialPbh_p_touched)
			p = int_camera_cache[offset].InitialPbh_p;
		if (int_camera_cache[offset].InitialPbh_p_touched)
			b = int_camera_cache[offset].InitialPbh_b;
		if (int_camera_cache[offset].InitialPbh_p_touched)
			h = int_camera_cache[offset].InitialPbh_h;
	}

	sprintf(rot, "%f, %f, %f", p,b,h);
	set_config_string(section, "InitialPbh", rot);

	if (offset == -1)
		return;
	else
		do_camera_other_adjustments(int_camera_cache, section, offset);
}

void do_camera_other_adjustments(camera* camera_cache, char* section, int offset)
{
	//do the other adjustments
	if (camera_cache[offset].description_touched)
	{
		set_config_string(section, "description", camera_cache[offset].description);
	}
	if (camera_cache[offset].showpanel_touched)
	{
		if (camera_cache[offset].showpanel)
			set_config_string(section, "showpanel", "Yes");
		else
			set_config_string(section, "showpanel", "No");
	}
	if (camera_cache[offset].snappbhadjust_touched)
	{
		set_config_string(section, "snappbhadjust", camera_cache[offset].snappbhadjust);
	}
	if (camera_cache[offset].panpbhadjust_touched)
	{
		set_config_string(section, "panpbhadjust", camera_cache[offset].panpbhadjust);
	}
	if (camera_cache[offset].snappbhreturn_touched)
	{
		if (camera_cache[offset].snappbhreturn)
			set_config_string(section, "snappbhreturn", "True");
		else
			set_config_string(section, "snappbhreturn", "False");
	}
	if (camera_cache[offset].panpbhreturn_touched)
	{
		if (camera_cache[offset].panpbhreturn)
			set_config_string(section, "panpbhreturn", "True");
		else
			set_config_string(section, "panpbhreturn", "False");
	}
	if (camera_cache[offset].track_touched)
	{
		set_config_string(section, "track", camera_cache[offset].track);
	}
	if (camera_cache[offset].showaxis_touched)
	{
		set_config_string(section, "showaxis", camera_cache[offset].showaxis);
	}
	if (camera_cache[offset].allowzoom_touched)
	{
		if (camera_cache[offset].allowzoom)
			set_config_string(section, "allowzoom", "Yes");
		else
			set_config_string(section, "allowzoom", "No");
	}
	if (camera_cache[offset].initialzoom_touched)
	{				
		set_config_float(section, "initialzoom", camera_cache[offset].initialzoom);
	}
	if (camera_cache[offset].smoothzoomtime_touched)
	{
		set_config_float(section, "smoothzoomtime", camera_cache[offset].smoothzoomtime);
	}
	if (camera_cache[offset].showweather_touched)
	{
		if (camera_cache[offset].showweather)
			set_config_string(section, "showweather", "Yes");
		else
			set_config_string(section, "showweather", "No");
	}
	if (camera_cache[offset].xyzAdjust_touched)
	{
		if (camera_cache[offset].xyzAdjust)
			set_config_string(section, "xyzAdjust", "True");
		else
			set_config_string(section, "xyzAdjust", "False");
	}
	if (camera_cache[offset].xyzRate_touched)
	{
		set_config_float(section, "xyzRate", camera_cache[offset].xyzRate);
	}
	if (camera_cache[offset].xyzAccelleratorTime_touched)
	{
		set_config_float(section, "xyzAccelleratorTime", camera_cache[offset].xyzAccelleratorTime);
	}
	if (camera_cache[offset].AllowPbhAdjust_touched)
	{
		if (camera_cache[offset].AllowPbhAdjust)
			set_config_string(section, "AllowPbhAdjust", "Yes");
		else
			set_config_string(section, "AllowPbhAdjust", "No");
	}
	if (camera_cache[offset].showlensflare_touched)
	{
		if (camera_cache[offset].showlensflare)
			set_config_string(section, "showlensflare", "True");
		else
			set_config_string(section, "showlensflare", "False");
	}
	if (camera_cache[offset].category_touched)
	{
		set_config_string(section, "category", camera_cache[offset].category);
	}
	if (camera_cache[offset].zoompanscalar_touched)
	{
		set_config_float(section, "zoompanscalar", camera_cache[offset].zoompanscalar);
	}
	if (camera_cache[offset].momentumeffect_touched)
	{
		if (camera_cache[offset].momentumeffect)
			set_config_string(section, "momentumeffect", "Yes");
		else
			set_config_string(section, "momentumeffect", "No");
	}
	if (camera_cache[offset].pitchpanrate_touched)
	{
		set_config_float(section, "pitchpanrate", camera_cache[offset].pitchpanrate);
	}
	if (camera_cache[offset].headingpanrate_touched)
	{
		set_config_float(section, "headingpanrate", camera_cache[offset].headingpanrate);
	}
	if (camera_cache[offset].panacceleratortime_touched)
	{
		set_config_float(section, "panacceleratortime", camera_cache[offset].panacceleratortime);
	}
	if (camera_cache[offset].hotkeyselect_touched)
	{
		set_config_string(section, "hotkeyselect", camera_cache[offset].hotkeyselect);
	}
	if (camera_cache[offset].transition_touched)
	{
		if (camera_cache[offset].transition)
			set_config_string(section, "transition", "Yes");
		else
			set_config_string(section, "transition", "No");
	}
	if (camera_cache[offset].clipmode_touched)
	{
		set_config_string(section, "clipmode", camera_cache[offset].clipmode);
	}
	if (camera_cache[offset].chasedistance_touched)
	{
		set_config_float(section, "chasedistance", camera_cache[offset].chasedistance);
	}
	if (camera_cache[offset].chaseheading_touched)
	{
		set_config_float(section, "chaseheading", camera_cache[offset].chaseheading);
	}
	if (camera_cache[offset].chasealtitude_touched)
	{
		set_config_float(section, "chasealtitude", camera_cache[offset].chasealtitude);
	}
	if (camera_cache[offset].chasetime_touched)
	{
		set_config_float(section, "chasetime", camera_cache[offset].chasetime);
	}
	if (camera_cache[offset].instancedbased_touched)
	{
		if (camera_cache[offset].instancedbased)
			set_config_string(section, "instancedbased", "Yes");
		else
			set_config_string(section, "instancedbased", "No");
	}
	if (camera_cache[offset].cyclehideradius_touched)
	{
		set_config_float(section, "cyclehideradius", camera_cache[offset].cyclehideradius);
	}
	if (camera_cache[offset].targetcategory_touched)
	{
		set_config_string(section, "targetcategory", camera_cache[offset].targetcategory);
	}
	if (camera_cache[offset].cyclehidden_touched)
	{
		if (camera_cache[offset].cyclehidden)
			set_config_string(section, "cyclehidden", "Yes");
		else
			set_config_string(section, "cyclehidden", "No");
	}
	if (camera_cache[offset].FixedLatitude_touched)
	{
		set_config_float(section, "FixedLatitude", camera_cache[offset].FixedLatitude);
	}
	if (camera_cache[offset].FixedLongitude_touched)
	{
		set_config_float(section, "FixedLongitude", camera_cache[offset].FixedLongitude);
	}
	if (camera_cache[offset].FixedAltitude_touched)
	{
		set_config_float(section, "FixedAltitude", camera_cache[offset].FixedAltitude);
	}
}	

void preload_camera_configs(camera* camera_cache, char* sec, char* cfgfile)
{
	last_config_examined[0] = '\0';	
	set_config_file(cfgfile);
	
	char section[80];
	int id = -1;
	camera default_values;
	for (int k = 0; k < 512; k++)
	{
		sprintf(section, "%s.%d", sec, k);
									
		if (exists_in_config(cfgfile, section, "InitialXyz_y"))
		{				
			camera_cache[k].identifier = k;
			camera_cache[k].InitialXyz_y_touched = 1;
			camera_cache[k].InitialXyz_y = get_config_float(section, "InitialXyz_y", default_values.InitialXyz_y);			
		}
		
		if (exists_in_config(cfgfile, section, "InitialXyz_z"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].InitialXyz_z_touched = 1;
			camera_cache[k].InitialXyz_z = -1*get_config_float(section, "InitialXyz_z", default_values.InitialXyz_z);			
		}

		if (exists_in_config(cfgfile, section, "InitialPbh_p"))
		{					
			camera_cache[k].identifier = k;
			camera_cache[k].InitialPbh_p_touched = 1;
			camera_cache[k].InitialPbh_p = get_config_float(section, "InitialPbh_p", default_values.InitialPbh_p);			
		}

		if (exists_in_config(cfgfile, section, "InitialPbh_b"))
		{					
			camera_cache[k].identifier = k;
			camera_cache[k].InitialPbh_b_touched = 1;
			camera_cache[k].InitialPbh_b = get_config_float(section, "InitialPbh_b", default_values.InitialPbh_b);			
		}
		
		if (exists_in_config(cfgfile, section, "InitialPbh_h"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].InitialPbh_h_touched = 1;
			camera_cache[k].InitialPbh_h = get_config_float(section, "InitialPbh_h", default_values.InitialPbh_h);			
		}

		if (exists_in_config(cfgfile, section, "guid"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].guid_touched = 1;
			strcpy(camera_cache[k].guid, get_config_string(section, "guid", default_values.guid));
		}

		if (exists_in_config(cfgfile, section, "title"))
		{				
			camera_cache[k].identifier = k;
			camera_cache[k].title_touched = 1;
			strcpy(camera_cache[k].title, get_config_string(section, "title", default_values.title));
		}

		if (exists_in_config(cfgfile, section, "description"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].description_touched = 1;
			strcpy(camera_cache[k].description, get_config_string(section, "description", default_values.description));
		}

		if (exists_in_config(cfgfile, section, "origin"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].origin_touched = 1;
			strcpy(camera_cache[k].origin, get_config_string(section, "origin", default_values.origin));			
		}		

		if (exists_in_config(cfgfile, section, "showpanel"))
		{		
			camera_cache[k].identifier = k;
			camera_cache[k].showpanel_touched = 1;
			camera_cache[k].showpanel = get_config_int(section, "showpanel", default_values.showpanel);			
		}

		if (exists_in_config(cfgfile, section, "snappbhadjust"))
		{		
			camera_cache[k].identifier = k;
			camera_cache[k].snappbhadjust_touched = 1;
			strcpy(camera_cache[k].snappbhadjust, get_config_string(section, "snappbhadjust", default_values.snappbhadjust));			
		}
		
		if (exists_in_config(cfgfile, section, "panpbhadjust"))
		{		
			camera_cache[k].identifier = k;
			camera_cache[k].panpbhadjust_touched = 1;
			strcpy(camera_cache[k].panpbhadjust, get_config_string(section, "panpbhadjust", default_values.panpbhadjust));			
		}

		if (exists_in_config(cfgfile, section, "snappbhreturn"))
		{		
			camera_cache[k].identifier = k;
			camera_cache[k].snappbhreturn_touched = 1;
			camera_cache[k].snappbhreturn = get_config_int(section, "snappbhreturn", default_values.snappbhreturn);			
		}

		if (exists_in_config(cfgfile, section, "panpbhreturn"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].panpbhreturn_touched = 1;
			camera_cache[k].panpbhreturn = get_config_int(section, "panpbhreturn", default_values.panpbhreturn);			
		}

		if (exists_in_config(cfgfile, section, "track"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].track_touched = 1;
			strcpy(camera_cache[k].track, get_config_string(section, "track", default_values.track));			
		}

		if (exists_in_config(cfgfile, section, "showaxis"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].showaxis_touched = 1;
			strcpy(camera_cache[k].showaxis, get_config_string(section, "showaxis", default_values.showaxis));			
		}

		if (exists_in_config(cfgfile, section, "allowzoom"))
		{		
			camera_cache[k].identifier = k;
			camera_cache[k].allowzoom_touched = 1;
			camera_cache[k].allowzoom = get_config_int(section, "allowzoom", default_values.allowzoom);			
		}
		
		if (exists_in_config(cfgfile, section, "initialzoom"))
		{					
			camera_cache[k].identifier = k;
			camera_cache[k].initialzoom_touched = 1;
			camera_cache[k].initialzoom = get_config_float(section, "initialzoom", default_values.initialzoom);			
		}

		if (exists_in_config(cfgfile, section, "smoothzoomtime"))
		{		
			camera_cache[k].identifier = k;
			camera_cache[k].smoothzoomtime_touched = 1;
			camera_cache[k].smoothzoomtime = get_config_float(section, "smoothzoomtime", default_values.smoothzoomtime);			
		}

		if (exists_in_config(cfgfile, section, "showweather"))
		{		
			camera_cache[k].identifier = k;
			camera_cache[k].showweather_touched = 1;
			camera_cache[k].showweather = get_config_int(section, "showweather", default_values.showweather);			
		}

		if (exists_in_config(cfgfile, section, "InitialXyz_x"))
		{		
			camera_cache[k].identifier = k;
			camera_cache[k].InitialXyz_x_touched = 1;
			camera_cache[k].InitialXyz_x = get_config_float(section, "InitialXyz_x", default_values.InitialXyz_x);			
		}
		
		if (exists_in_config(cfgfile, section, "xyzAdjust"))
		{		
			camera_cache[k].identifier = k;
			camera_cache[k].xyzAdjust_touched = 1;
			camera_cache[k].xyzAdjust = get_config_int(section, "xyzAdjust", default_values.xyzAdjust);			
		}

		if (exists_in_config(cfgfile, section, "xyzRate"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].xyzRate_touched = 1;
			camera_cache[k].xyzRate = get_config_float(section, "xyzRate", default_values.xyzRate);			
		}

		if (exists_in_config(cfgfile, section, "xyzAccelleratorTime"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].xyzAccelleratorTime_touched = 1;
			camera_cache[k].xyzAccelleratorTime = get_config_float(section, "xyzAccelleratorTime", default_values.xyzAccelleratorTime);			
		}

		if (exists_in_config(cfgfile, section, "AllowPbhAdjust"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].AllowPbhAdjust_touched = 1;
			camera_cache[k].AllowPbhAdjust = get_config_int(section, "AllowPbhAdjust", default_values.AllowPbhAdjust);			
		}
		
		if (exists_in_config(cfgfile, section, "showlensflare"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].showlensflare_touched = 1;
			camera_cache[k].showlensflare = get_config_int(section, "showlensflare", default_values.showlensflare);			
		}
		
		if (exists_in_config(cfgfile, section, "category"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].category_touched = 1;
			strcpy(camera_cache[k].category, get_config_string(section, "category", default_values.category));			
		}

		if (exists_in_config(cfgfile, section, "zoompanscalar"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].zoompanscalar_touched = 1;
			camera_cache[k].zoompanscalar = get_config_float(section, "zoompanscalar", default_values.zoompanscalar);			
		}

		if (exists_in_config(cfgfile, section, "momentumeffect"))
		{			
			camera_cache[k].identifier = k;
			camera_cache[k].momentumeffect_touched = 1;
			camera_cache[k].momentumeffect = get_config_int(section, "momentumeffect", default_values.momentumeffect);			
		}

		if (exists_in_config(cfgfile, section, "pitchpanrate"))
		{				
			camera_cache[k].identifier = k;
			camera_cache[k].pitchpanrate_touched = 1;
			camera_cache[k].pitchpanrate = get_config_float(section, "pitchpanrate", default_values.pitchpanrate);			
		}
		
		if (exists_in_config(cfgfile, section, "headingpanrate"))
		{				
			camera_cache[k].identifier = k;
			camera_cache[k].headingpanrate_touched = 1;
			camera_cache[k].headingpanrate = get_config_float(section, "headingpanrate", default_values.headingpanrate);			
		}

		if (exists_in_config(cfgfile, section, "panacceleratortime"))
		{				
			camera_cache[k].identifier = k;
			camera_cache[k].panacceleratortime_touched = 1;
			camera_cache[k].panacceleratortime = get_config_float(section, "panacceleratortime", default_values.panacceleratortime);			
		}

		if (exists_in_config(cfgfile, section, "hotkeyselect"))
		{				
			camera_cache[k].identifier = k;
			camera_cache[k].hotkeyselect_touched = 1;
			strcpy(camera_cache[k].hotkeyselect, get_config_string(section, "hotkeyselect", default_values.hotkeyselect));			
		}

		if (exists_in_config(cfgfile, section, "transition"))
		{				
			camera_cache[k].identifier = k;
			camera_cache[k].transition_touched = 1;
			camera_cache[k].transition = get_config_int(section, "transition", default_values.transition);			
		}
		
		if (exists_in_config(cfgfile, section, "clipmode"))
		{				
			camera_cache[k].identifier = k;
			camera_cache[k].clipmode_touched = 1;
			strcpy(camera_cache[k].clipmode, get_config_string(section, "clipmode", default_values.clipmode));			
		}

		if (exists_in_config(cfgfile, section, "chasedistance"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].chasedistance_touched = 1;
			camera_cache[k].chasedistance = get_config_float(section, "chasedistance", default_values.chasedistance);			
		}
		if (exists_in_config(cfgfile, section, "chaseheading"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].chaseheading_touched = 1;
			camera_cache[k].chaseheading = get_config_float(section, "chaseheading", default_values.chaseheading);			
		}
		if (exists_in_config(cfgfile, section, "chasealtitude"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].chasealtitude_touched = 1;
			camera_cache[k].chasealtitude = get_config_float(section, "chasealtitude", default_values.chasealtitude);			
		}
		if (exists_in_config(cfgfile, section, "chasetime"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].chasetime_touched = 1;
			camera_cache[k].chasetime = get_config_float(section, "chasetime", default_values.chasetime);			
		}

		if (exists_in_config(cfgfile, section, "instancedbased"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].instancedbased_touched = 1;
			camera_cache[k].instancedbased = get_config_int(section, "instancedbased", default_values.instancedbased);			
		}
		if (exists_in_config(cfgfile, section, "cyclehideradius"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].cyclehideradius_touched = 1;
			camera_cache[k].cyclehideradius = get_config_float(section, "cyclehideradius", default_values.cyclehideradius);			
		}

		if (exists_in_config(cfgfile, section, "targetcategory"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].targetcategory_touched = 1;
			strcpy(camera_cache[k].targetcategory, get_config_string(section, "targetcategory", default_values.targetcategory));			
		}
		if (exists_in_config(cfgfile, section, "cyclehidden"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].cyclehidden_touched = 1;
			camera_cache[k].cyclehidden = get_config_float(section, "cyclehidden", default_values.cyclehidden);			
		}

		if (exists_in_config(cfgfile, section, "FixedLatitude"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].FixedLatitude_touched = 1;
			camera_cache[k].FixedLatitude = get_config_float(section, "FixedLatitude", default_values.FixedLatitude);			
		}
		if (exists_in_config(cfgfile, section, "FixedLongitude"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].FixedLongitude_touched = 1;
			camera_cache[k].FixedLongitude = get_config_float(section, "FixedLongitude", default_values.FixedLongitude);			
		}
		if (exists_in_config(cfgfile, section, "FixedAltitude"))
		{						
			camera_cache[k].identifier = k;
			camera_cache[k].FixedAltitude_touched = 1;
			camera_cache[k].FixedAltitude = get_config_float(section, "FixedAltitude", default_values.FixedAltitude);			
		}
	}	
}

char last_config_examined[1024];
struct txtline
{
	char data[1024];
	txtline() {data[0] = '\0';};
};
txtline cfg_lines[10240];
int num_cfg_lines = 0;
int exists_in_config(char* cfgfile, char* section, char* key)
{
	if (stricmp(cfgfile, last_config_examined))
	{
		strcpy(last_config_examined, cfgfile);
		num_cfg_lines = 0;
		FILE* file = fopen(cfgfile, "rt");
		if (!file)
			return 0;
		while(1)
		{
			fgets(cfg_lines[num_cfg_lines].data, 1024, file);						
			if (feof(file))
			{	
				break;
			}
				
			num_cfg_lines++;
		}
		fclose(file);
	}
			
	int currline = 0;
	//look for our section
	char secname[1024];
	sprintf(secname, "[%s]", section);
	_strupr(secname);
	int found_context = 0;
	int found_key = 0;	
	char tempstr[1024];
	char tempkey[1024];
	strcpy(tempkey, key);
	_strupr(tempkey);

	while(1)
	{
		if (currline >= num_cfg_lines)
			break;
		

		strcpy(tempstr, cfg_lines[currline].data);
		_strupr(tempstr);
		if (strstr(tempstr, secname))
		{		
			found_context = 1;			
			break;
		}		
		
		currline++;
	}

	if (!found_context)
	{					
		return 0;
	}
	
	//now look for the key in this context
	
	while(1)
	{
		currline++;	
		//printf("%d:%s", currline, cfg_lines[currline].data);

		//fgets(buff, 1024, file);
		if (currline > num_cfg_lines)
		{			
			break;
		}		

		strcpy(tempstr, cfg_lines[currline].data);
		_strupr(tempstr);
		if (strstr(tempstr, "["))
		{					
			break;
		}
		if (strstr(tempstr, tempkey))
		{
			found_key = 1;
			break;
		}

	
	}	
	
	return found_key;	
}

void do_aircraft_cfg(char* line, FILE* in)
{	

	ext_camera_count = 0;
	int_camera_count = 0;
	//is this an engine def	
	char match[80];

	//_contact
	//_wheel
	//_scrape
	//_skid
	//_float
	//_waterrudder

	//////////////////////////
	//FUEL STATIONS
	//////////////////////////
	if (strstr(line, ": _fueltank_center1"))
	{
		process_fuel_tank(CENTER1, in, "center1", tank_cache[CENTER1].usable, tank_cache[CENTER1].unusable);
	}
	if (strstr(line, ": _fueltank_center2"))
	{
		process_fuel_tank(CENTER2, in, "center2", tank_cache[CENTER2].usable, tank_cache[CENTER2].unusable);
	}	
	if (strstr(line, ": _fueltank_center3"))
	{
		process_fuel_tank(CENTER3, in, "center3", tank_cache[CENTER3].usable, tank_cache[CENTER3].unusable);
	}
	if (strstr(line, ": _fueltank_leftmain"))
	{
		process_fuel_tank(LEFTMAIN, in, "leftmain", tank_cache[LEFTMAIN].usable, tank_cache[LEFTMAIN].unusable);
	}
	if (strstr(line, ": _fueltank_rightmain"))
	{
		process_fuel_tank(RIGHTMAIN, in, "rightmain", tank_cache[RIGHTMAIN].usable, tank_cache[RIGHTMAIN].unusable);
	}
	if (strstr(line, ": _fueltank_leftaux"))
	{
		process_fuel_tank(LEFTAUX, in, "leftaux", tank_cache[LEFTAUX].usable, tank_cache[LEFTAUX].unusable);
	}
	if (strstr(line, ": _fueltank_rightaux"))
	{
		process_fuel_tank(RIGHTAUX, in, "rightaux", tank_cache[RIGHTAUX].usable, tank_cache[RIGHTAUX].unusable);
	}
	if (strstr(line, ": _fueltank_lefttip"))
	{
		process_fuel_tank(LEFTTIP, in, "lefttip", tank_cache[LEFTTIP].usable, tank_cache[LEFTTIP].unusable);
	}
	if (strstr(line, ": _fueltank_righttip"))
	{
		process_fuel_tank(RIGHTTIP, in, "righttip", tank_cache[RIGHTTIP].usable, tank_cache[RIGHTTIP].unusable);
	}
	if (strstr(line, ": _fueltank_external1"))
	{
		process_fuel_tank(EXTERNAL1, in, "external1", tank_cache[EXTERNAL1].usable, tank_cache[EXTERNAL1].unusable);
	}
	if (strstr(line, ": _fueltank_external2"))
	{
		process_fuel_tank(EXTERNAL2, in, "external2", tank_cache[EXTERNAL2].usable, tank_cache[EXTERNAL2].unusable);
	}
	//////////////////////////
	//END FUEL STATIONS
	//////////////////////////
	if (strstr(line, ": _gearc"))
	{	
		int offset = extract_number(line);
		process_gearc(in, offset);
	}

	if (strstr(line, ": _gearu"))
	{	
		int offset = extract_number(line);
		process_gearu(in, offset);
	}

	if (strstr(line, ": _secondaryrotor"))
	{			
		process_secondary_rotor(in);
	}

	if (strstr(line, ": _mainrotor"))
	{			
		process_main_rotor(in);
	}

	if (strstr(line, ": _anemometer"))
	{	
		int offset = extract_number(line);
		process_anemometers(in, offset);
	}

	if (strstr(line, ": _ballast"))
	{	
		int offset = extract_number(line);
		process_ballast(in, offset);
	}

	if (strstr(line, ": _smoke"))
	{	
		int offset = extract_number(line);
		process_smoke(in, offset);
	}

	if (strstr(line, ": _liftaerocenter"))
	{	
		process_lift_aero_center(in);
	}

	if (strstr(line, ": _emptycg"))
	{	
		process_empty_cg(in);
	}

	if (strstr(line, ": _contact"))
	{			
		int offset = extract_number(line);
		process_contact(in, offset, 0);
	}
	
	/*if (strstr(line, ": _contactwheel"))
	{	
		int offset = extract_number(line);
		process_contact(in, offset, 1);
	}
	if (strstr(line, ": _contactscrape"))
	{	
		int offset = extract_number(line);
		process_contact(in, offset, 2);
	}
	if (strstr(line, ": _contactskid"))
	{	
		int offset = extract_number(line);
		process_contact(in, offset, 3);
	}
	if (strstr(line, ": _contactfloat"))
	{	
		int offset = extract_number(line);
		process_contact(in, offset, 4);
	}
	if (strstr(line, ": _contactwaterrudder"))
	{	
		int offset = extract_number(line);
		process_contact(in, offset, 5);
	}

	if (strstr(line, ": _eyepoint"))
	{	
		process_pilot_view(in);
	}
*/
	for (int k = 0; k < 12; k++)
	{
		sprintf(match, ": _engine_%d", k);
		if (strstr(line, match))
		{			
			process_engine(in, k);			
		}
	}
	
	if (strstr(line, ": _exit"))
	{		
		int offset = extract_number(line);
		process_exits(in, offset);		
	}

	if (strstr(line, ": _stationload"))
	{		
		int offset = extract_number(line);
		process_stations(in, offset);		
	}

	if (strstr(line, ": _camera"))
	{		
		process_ext_cam(line, in, ext_camera_count);
		ext_camera_count++;
	}

	if (strstr(line, ": _vccamera"))
	{		
		process_int_cam(line, in, int_camera_count);
		int_camera_count++;
	}

}



void build_guid(char* result)
{
	//a guid has this format:
	//{5c1df273-034b-4e7f-953a-9d5e26f1646c}
	//8 hex - 4 hex - 4 hex - 4 hex - 12 hex
	
	char* legal = "01234567890abcdef";
	int legal_length = 16;
	int pos = 0;
	result[pos] = '{';
	pos++;
	for (int k = 0; k <  8; k++)
	{
		result[pos] = legal[rand()%16];
		pos++;
	}
	result[pos] = '-';
	pos++;
	for (int k = 0; k <  4; k++)
	{
		result[pos] = legal[rand()%16];
		pos++;
	}
	result[pos] = '-';
	pos++;
	for (int k = 0; k <  4; k++)
	{
		result[pos] = legal[rand()%16];
		pos++;
	}
	result[pos] = '-';
	pos++;
	for (int k = 0; k <  4; k++)
	{
		result[pos] = legal[rand()%16];
		pos++;
	}
	result[pos] = '-';
	pos++;
	for (int k = 0; k <  12; k++)
	{
		result[pos] = legal[rand()%16];
		pos++;
	}
	result[pos] = '}';
	pos++;
	result[pos] = '\0';
}

void section_spaces_hack_backward(char* input, char* output)
{
	//look for [ReferenceSpeeds] and replace it with [Reference Speeds]
	//look for [WaterBallastSystem] and replace with [Water Ballast System]

	FILE* in = fopen(input, "rt");
	FILE* out = fopen(output, "wt");
	char buff[1024];	
	char up[1024];	
	int write = 1;
	while(1)
	{
		fgets(buff, 1024, in);		
		if (feof(in))
			break;
		write = 1;
		strcpy(up, buff);
		_strupr(up);
		if (strstr(up, "[REFERENCESPEEDS]"))
		{
			fprintf(out, "[Reference Speeds]\n");
			write = 0;
		}
		if (strstr(up, "[WATERBALLASTSYSTEM]"))
		{
			fprintf(out, "[Water Ballast System]\n");
			write = 0;
		}

		if (write)
		{
			fprintf(out, "%s", buff);
		}
	}

	fclose(in);
	fclose(out);
}

void section_spaces_hack_forward(char* input, char* output)
{
	//look for [Reference Speeds] and replace it with [ReferenceSpeeds]
	//look for [Water Ballast System] and replace with [WaterBallastSystem]

	FILE* in = fopen(input, "rt");
	FILE* out = fopen(output, "wt");
	char buff[1024];	
	char up[1024];	
	int write = 1;
	while(1)
	{
		fgets(buff, 1024, in);		
		if (feof(in))
			break;
		write = 1;
		strcpy(up, buff);
		_strupr(up);
		if (strstr(up, "[REFERENCE SPEEDS]"))
		{
			fprintf(out, "[REFERENCESPEEDS]\n");
			write = 0;
		}
		if (strstr(up, "[WATER BALLAST SYSTEM]"))
		{
			fprintf(out, "[WATERBALLASTSYSTEM]\n");
			write = 0;
		}

		if (write)
		{
			fprintf(out, "%s", buff);
		}
	}

	fclose(in);
	fclose(out);
}

void copy_file(char* input, char* output)
{
	FILE* in = fopen(input, "rb");
	FILE* out = fopen(output, "wb");
	char c;	

	while(1)
	{
		c = fgetc(in);
		if (feof(in))
			break;
		fputc(c, out);
	}
	fclose(in);
	fclose(out);
}