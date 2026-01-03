#define FEET_TO_METER 3.2808399

struct ModelParams
{
	int ShadowMapReady;
	int ShadowMap_temp_ok;
	int ShadowMap_entry_ok;	
	int UnitsMetric;
	float eyepoint_x;
	float eyepoint_y;
	float eyepoint_z;
	float eyepoint_metric_x;
	float eyepoint_metric_y;
	float eyepoint_metric_z;

	float drp_x;
	float drp_y;
	float drp_z;
	
	ModelParams()
	{
		UnitsMetric = 0;
		ShadowMapReady = 0;
		ShadowMap_temp_ok = 0;
		ShadowMap_entry_ok = 0;
		eyepoint_x = 0;
		eyepoint_y = 0;
		eyepoint_z = 0;
		eyepoint_metric_x = 0;
		eyepoint_metric_y = 0;
		eyepoint_metric_z = 0;
		drp_x = 0;
		drp_y = 0;
		drp_z = 0;

	};
};

struct FS10Material
{
	char identifier[1024];
	
	float diffuse_r;
	float diffuse_g;
	float diffuse_b;
	float diffuse_a;
	int diffuse_ok;

	float specular_r;
	float specular_g;
	float specular_b;
	float specular_p;
	int specular_ok;

	char diffuse[1024];
	char specular[1024];
	char ambient[1024];
	char emissive[1024];
	char reflection[1024];
	char shininess[1024];
	char bump[1024];
	char displacement[1024];
	char detail[1024];
	char fresnel[1024];		

	float EnvironmentLevelScale;
	float detail_scale;
	float bump_scale;

	//enhanced parameters
	//EnhancedParameters  {
    //  0;  // Assume vertical normal
    //  0;  // Z-Write alpha
    //  0;  // No Z-Write 
    //  0;  // Volume shadow
    //  0;  // No shadow
    //  0;  // Prelit vertices
    //}
	int ep_assume_vertical_normal;
	int ep_z_write_alpha;
	int ep_no_z_write;
	int ep_volume_shadow;
	int ep_no_shadow;
	int ep_prelit_vertices;

	//material parameters
	// Use global environment
	// Do not blend env by inv diffuse alpha
	// Do not blend env by specular map alpha
	// Fresnel - diffuse = NO, Specular = YES, Env = YES
	// Precipitation - Use = NO, Offset = NO, Offset(f)
	// Specular Map Power Scale(f)
	int mp_use_global_env;
	int mp_blend_env_by_inv_diff_alpha;
	int mp_blend_env_by_specular_alpha;
	int mp_fresnel_diffuse;
	int mp_fresnel_specular;
	int mp_fresnel_env;
	int mp_precip_use;
	int mp_precip_offset;
	float mp_precip_offset_value;
	float mp_specular_map_power_scale;

	char SrcBlend[80];
	int SrcBlendFLAG;
	char DestBlend[80];
	int DestBlendFLAG;
	char EmissiveMode[80];
	int EmissiveModeFLAG;

	//misc stuff
	int misc_isnnumber;
	int misc_doublesided;

	//bloom stuff
	int bloom_allowbloom;
	int bloom_emissivebloom;
	int bloom_nospecularbloom;
	int bloom_bloombycopy;
	int bloom_bloommodulatingbyalpha;
	float bloom_ambientlightscale;
	float bloom_specularbloomfloor;
	
	//v2.8 properties
	int BlendDiffuseByInverseSpecularMapAlpha;
	int BlendDiffuseByBaseAlpha;
	int ForceTextureAddressWrapSetting;
	int ForceTextureAddressClamp;
	int	Alpha_ZTestAlpha;
	float Alpha_AlphaTestValue;
	char Alpha_AlphaTestFunction[80];
	int Alpha_AlphaTestFunctionFLAG;
	bool Alpha_FinalAlphaWrite;
	float Alpha_FinalAlphaWriteValue;

	FS10Material()
	{
		identifier[0] = '\0';

		diffuse_r = -1;
		diffuse_g = -1;
		diffuse_b = -1;
		diffuse_a = -1;

		specular_r = -1;
		specular_g = -1;
		specular_b = -1;
		specular_p = -1;

		diffuse_ok = 0;
		specular_ok = 0;

		diffuse[0] = '\0';
		specular[0] = '\0';
		ambient[0] = '\0';
		emissive[0] = '\0';
		reflection[0] = '\0';
		shininess[0] = '\0';
		bump[0] = '\0';
		displacement[0] = '\0';
		detail[0] = '\0';
		fresnel[0] = '\0';

		EnvironmentLevelScale = 0;
		detail_scale = 0;
		bump_scale = 0;

		ep_assume_vertical_normal = 0;
		ep_z_write_alpha = 0;
		ep_no_z_write = 0;
		ep_volume_shadow = 0;
		ep_no_shadow = 0;
		ep_prelit_vertices = 0;

		mp_use_global_env = 0;
		mp_blend_env_by_inv_diff_alpha = 0;
		mp_blend_env_by_specular_alpha = 0;
		mp_fresnel_diffuse = 0;
		mp_fresnel_specular = 0;
		mp_fresnel_env = 0;
		mp_precip_use = 0;
		mp_precip_offset = 0;
		mp_precip_offset_value = 0;
		mp_specular_map_power_scale = 0;

		SrcBlend[0] = '\0';
		DestBlend[0] = '\0';
		EmissiveMode[0] = '\0';
		SrcBlendFLAG = -1;
		DestBlendFLAG = -1;
		EmissiveModeFLAG = -1;

		//misc stuff
		misc_isnnumber = 0;
		misc_doublesided = 0;

		//bloom stuff
		bloom_allowbloom = -1;
		bloom_emissivebloom = 0;
		bloom_nospecularbloom = 0;
		bloom_bloombycopy = 0;
		bloom_bloommodulatingbyalpha = 0;
		bloom_ambientlightscale = -1;
		bloom_specularbloomfloor = -1;

		//v2.8 stuff
		BlendDiffuseByInverseSpecularMapAlpha = 0;
		BlendDiffuseByBaseAlpha = 0;
	    ForceTextureAddressWrapSetting = 0;
	    ForceTextureAddressClamp = 0;
	    Alpha_ZTestAlpha = 0;
	    Alpha_AlphaTestValue = -1;
		Alpha_AlphaTestFunction[0] = '\0';
		Alpha_AlphaTestFunctionFLAG = 0;
		Alpha_FinalAlphaWrite = 0;
		Alpha_FinalAlphaWriteValue = -1;
	};
};


struct camera
{
	int identifier;
	char title[512];
	char guid[512];
	char description[512];
	char origin[512];
	int showpanel;
	char snappbhadjust[512];
	char panpbhadjust[512];
	int snappbhreturn;
	int panpbhreturn;
	char track[512];
	char showaxis[512];
	int allowzoom;
	float initialzoom;
	float smoothzoomtime;
	int	showweather;
	float InitialXyz_x;
	float InitialXyz_y;
	float InitialXyz_z;
	float InitialPbh_p;
	float InitialPbh_b;
	float InitialPbh_h;
	int xyzAdjust;
	float xyzRate;
	float xyzAccelleratorTime;
	int AllowPbhAdjust;
	int showlensflare;
	char category[512];
	float zoompanscalar;
	int momentumeffect;
	float pitchpanrate;
	float headingpanrate;
	float panacceleratortime;
	char hotkeyselect[512];
	int transition;
	char clipmode[512];
	float chasedistance;
	float chaseheading;
	float chasealtitude;
	float chasetime;
	int instancedbased;
	float cyclehideradius;
	char targetcategory[512];
	int cyclehidden;
	float FixedLatitude;
	float FixedLongitude;
	float FixedAltitude;

	int title_touched;
	int guid_touched;
	int description_touched;
	int origin_touched;
	int showpanel_touched;
	int snappbhadjust_touched;
	int panpbhadjust_touched;
	int snappbhreturn_touched;
	int panpbhreturn_touched;
	int track_touched;
	int showaxis_touched;
	int allowzoom_touched;
	int initialzoom_touched;
	int smoothzoomtime_touched;
	int showweather_touched;
	int InitialXyz_x_touched;
	int InitialXyz_y_touched;
	int InitialXyz_z_touched;
	int InitialPbh_p_touched;
	int InitialPbh_b_touched;
	int InitialPbh_h_touched;
	int xyzAdjust_touched;
	int xyzRate_touched;
	int xyzAccelleratorTime_touched;
	int AllowPbhAdjust_touched;
	int showlensflare_touched;
	int category_touched;
	int zoompanscalar_touched;
	int momentumeffect_touched;
	int pitchpanrate_touched;
	int headingpanrate_touched;
	int panacceleratortime_touched;
	int hotkeyselect_touched;
	int transition_touched;
	int clipmode_touched;
	int chasedistance_touched;
	int chaseheading_touched;
	int chasealtitude_touched;
	int chasetime_touched;
	int instancedbased_touched;
	int cyclehideradius_touched;
	int targetcategory_touched;
	int cyclehidden_touched;
	int FixedLatitude_touched;
	int FixedLongitude_touched;
	int FixedAltitude_touched;


	void clear()
	{			
	identifier = -1;
	guid[0] = '\0';
	title[0] = '\0';
	guid[0] = '\0';
	description[0] = '\0';
	origin[0] = '\0';
	showpanel = 0;
	snappbhadjust[0] = '\0';
	panpbhadjust[0] = '\0';
	snappbhreturn = 0;
	panpbhreturn = 0;
	track[0] = '\0';
	showaxis[0] = '\0';
	allowzoom = 1;
	initialzoom = -1;
	smoothzoomtime = -1;
	showweather = 0;
	InitialXyz_x = -9999;
	InitialXyz_y = -9999;
	InitialXyz_z = -9999;
	InitialPbh_p = -9999;
	InitialPbh_b = -9999;
	InitialPbh_h = -9999;
	xyzAdjust = 1;
	xyzRate = -9999;
	xyzAccelleratorTime = -9999;
	AllowPbhAdjust = 1;
	showlensflare = 0;
	category[0] = '\0';
	zoompanscalar = -9999;
	momentumeffect = 0;
	pitchpanrate = -9999;
	headingpanrate = -9999;
	panacceleratortime = -9999;
	hotkeyselect[0] = '\0';
	transition = 0;
	clipmode[0] = '\0';
	chasedistance = -9999;
	chaseheading = -9999;
	chasealtitude = -9999;
	chasetime = -9999;
	instancedbased = 0;
	cyclehideradius = -9999;
	targetcategory[0] = '\0';
	cyclehidden = 0;
	FixedLatitude = -9999;
	FixedLongitude = -9999;
	FixedAltitude = -9999;

	title_touched = 0;
	guid_touched = 0;
	guid_touched = 0;
	description_touched = 0;
	origin_touched = 0;
	showpanel_touched = 0;
	snappbhadjust_touched = 0;
	panpbhadjust_touched = 0;
	snappbhreturn_touched = 0;
	panpbhreturn_touched = 0;
	track_touched = 0;
	showaxis_touched = 0;
	allowzoom_touched = 0;
	initialzoom_touched = 0;
	smoothzoomtime_touched = 0;
	showweather_touched = 0;
	InitialXyz_x_touched = 0;
	InitialXyz_y_touched = 0;
	InitialXyz_z_touched = 0;
	InitialPbh_p_touched = 0;
	InitialPbh_b_touched = 0;
	InitialPbh_h_touched = 0;
	xyzAdjust_touched = 0;
	xyzRate_touched = 0;
	xyzAccelleratorTime_touched = 0;
	AllowPbhAdjust_touched = 0;
	showlensflare_touched = 0;
	category_touched = 0;
	zoompanscalar_touched = 0;
	momentumeffect_touched = 0;
	pitchpanrate_touched = 0;
	headingpanrate_touched = 0;
	panacceleratortime_touched = 0;
	hotkeyselect_touched = 0;
	transition_touched = 0;
	clipmode_touched = 0;
	chasedistance_touched = 0;
	chaseheading_touched = 0;
	chasealtitude_touched = 0;
	chasetime_touched = 0;
	instancedbased_touched = 0;
	cyclehideradius_touched = 0;
	targetcategory_touched = 0;
	cyclehidden_touched = 0;
	FixedLatitude_touched = 0;
	FixedLongitude_touched = 0;
	FixedAltitude_touched = 0;
	}

	camera()
	{
		clear();
	}

};

class contact
{
public:
	int Class;
	float impact_damage_threshold;
	int brake_map;
	float wheel_radius;
	float steering_angle;
	float static_compression;
	float max_static_compression_ratio;
	float damping_ratio;
	float extension_time;
	float retraction_time;
	int sound_type;
	float airspeed_limit;
	float damage_airspeed;

	float compressed_y;
	float uncompressed_y;
	
	int used;
	float x, y, z;
	float static_percent;

	contact()
	{
		static_percent = 0;
		used = 0;
		x = y = z = 0;
		Class = 0;
		impact_damage_threshold = 0;
		brake_map = 0;
		wheel_radius = 0;
		steering_angle = 0;
		static_compression = 0;
		max_static_compression_ratio = 0;
		damping_ratio = 0;
		extension_time = 0;
		retraction_time = 0;
		sound_type = 0;
		airspeed_limit = 0;
		damage_airspeed = 0;	
		compressed_y = 0;
		uncompressed_y = 0;
	};
};

class global_contact
{
public:
	int max_number_of_points;
	float static_pitch;
	float static_cg_height;
	int gear_system_type;
	int emergency_extension_type;
	int tailwheel_lock;
	int process;
	void clear()
	{
		process = 0;
		max_number_of_points = -1;
		static_pitch = -1;
		static_cg_height = -1;
		gear_system_type = -1;
		emergency_extension_type = -1;
		tailwheel_lock = -1;
	}

	global_contact()
	{
		clear();
	}
};
struct weight_station
{
	float x, y, z;
	char name[1024];
	float weight;

	weight_station()
	{
		x = y = z = 0;
		name[0] = '\0';
		weight = 0;
	}
};

struct fs_exit
{
	float x, y, z;
	float speed;
	int type;

	fs_exit()
	{
		x = y = z = 0;
		speed = -1;
		type = 0;
	}
};

struct fueltank
{
	float x, y, z;
	int inuse;
	float usable;
	float unusable;

	fueltank()
	{
		x = y = z = 0;
		inuse = 0;
		usable = 150;
		unusable = 5;
	}
};

#define CENTER1   0
#define CENTER2	  1
#define CENTER3   2
#define LEFTMAIN  3
#define LEFTAUX   4
#define LEFTTIP   5
#define RIGHTMAIN 6
#define RIGHTAUX  7
#define RIGHTTIP  8
#define EXTERNAL1 9
#define EXTERNAL2 10

struct smoke
{
	float x, y, z;
	char effect[1024];

	smoke()
	{
		x = y = z = 0;
		effect[0] = '\0';
	}
};

struct ballast
{
	float x, y, z;
	float capacity;
	int valve_index;

	ballast()
	{
		x = y = z = 0;
		capacity = 10;
		valve_index = 1;
	};
};


extern ballast ballast_cache[128];

extern smoke smoke_cache[128];
extern fueltank tank_cache[11];

extern weight_station station_cache[128];
extern int stations_used;

extern fs_exit exit_cache[128];
extern int exits_used;

extern contact gear_cache[128];
extern int gears_used;

extern contact contact_cache[128];
extern int contacts_used;
extern global_contact globalcontact;

extern int compat352;

void adjust_src_dst_strings(char* buff, int src, int* SrcBlendFLAG, int* DestBlendFLAG);
void adjust_emissive_strings(char* buff, int* emissivemodeflag);
void divide_blends(char* buff, char* sb, char* db);

void read_globalcontact();
void read_texture_parameters();
int read_model_parameters();
void read_station_parameters();
void write_texture_parameters();
void do_aircraft_cfg(char* line, FILE* in);

int audit_x_file(char* filename);
int produce_map_histo(char* input, char* _output);

void build_guid(char* result);
extern int ext_camera_count;
extern int int_camera_count;
void preload_camera_configs(camera* camera_cache, char* section, char* cfgfile);
extern char last_config_examined[1024];
int exists_in_config(char* cfgfile, char* section, char* key);
void do_camera_other_adjustments(camera* camera_cache, char* section, int offset);
void get_eyepoint(char* cfg);
void copy_file(char* input, char* output);
void section_spaces_hack_forward(char* input, char* output);
void section_spaces_hack_backward(char* input, char* output);
void process_datum_reference_point(FILE* in);
void process_stations(FILE* in, int station_offset);
void process_exits(FILE* in, int exit_offset);
void process_anemometers(FILE* in, int anemometer_offset);
void process_globalcontact();