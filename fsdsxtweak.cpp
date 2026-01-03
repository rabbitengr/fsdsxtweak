// fsdsxtweak.cpp : Defines the entry point for the console application.
//

//3.0 - support FSDS v3.5.2. They define their materials now at the top of the file rather than at each material list (to prevent dupes, I guess);
//but more importantly, they now do not put an FS10Material block at all in there - that means our trick of searching for 'FS10Material {' and replacing values
//no longer works. Instead we need to build the block from scratch. This would make it a breaking change with 3.5.1, so we add a '/352' command line switch
//on which we branch to see if we are going to replace (351 compat) or write from scratch (352 compat)
// The top of an FS10Material block looks like:
//FS10Material {
//	0.1; 0.1; 0.1; 1.0;;     <--- NOTE we need to read these three lines from the FS9 material present in the file (In FS9 mat the order is ABC, in FS10 it is ACB
//	0.2; 0.2; 0.2;;
//	0.0;
//	1.000000; 1.000000;	// Detail and bump scales
//	0.190000;     // Scale environment level factor
//	0;    // Use global environment
//	0;     // Do not blend env by inv diffuse alpha
//	0;    // Do not blend env by specular map alpha
//	0; 1; 1;    //Fresnel - diffuse = NO, Specular = YES, Env = YES
//	0; 0; 0.000000;    // Precipitation - Use = NO, Offset = NO
//	64.000000;    // Specular Map Power Scale
//	"One"; "Zero";  // Src/Dest blend
//	EmissiveData {
//		"Additive";	// Emissive Mode
//	}


//TODO (2.8): we have a bug - you can't have two parts with the same effect because of the part name disambiguation in fsds. We fix this by simply
// removing the _N from the end of the effect name - this means we can't have an effect with _N at the end, but oh well.

//TODO: allow you to run with just 2 cl args (.x in .x out) - for folks who dont want to tweak, just do the attach stuff

//DONE TODO - /KEEP /NOGUI /BATCH /NOCRASH /BMP2DDS /XMLSAMPLE
//DONE TODO for 2.7 - attachpoint stuff. Add these special parts:
//                                          _attachbyname_[name] <?xml version="1.0" encoding="ISO-8859-1" ?> <FSMakeMdlData version="9.0"> <Attachpoint name="%s"> </Attachpoint> </FSMakeMdlData>
//(need to generate the name automatically) _attachbyguid_[guid]        <?xml version="1.0" encoding="ISO-8859-1" ?> <FSMakeMdlData version="9.0"> <Attachpoint name="%s"> <AttachedObject> <LibraryObject name="%s" scale="1.0"/> </AttachedObject> </Attachpoint> </FSMakeMdlData>
//(need to generate the name automatically) _attacheffect_[effect name] <?xml version="1.0" encoding="ISO-8859-1" ?> <FSMakeMdlData version="9.0"> <Attachpoint name="%s"> <AttachedObject> <Effect effectName="%s" effectParams="EffectParams"/> </AttachedObject> </Attachpoint> </FSMakeMdlData>
//(need to generate the name automatically) _attachplatform_[surface type] <?xml version="1.0" encoding="ISO-8859-1" ?> <FSMakeMdlData version="9.0"> <Platform name="%s" surfaceType="%s" > </Platform> </FSMakeMdlData>


//do this in the do_model function. We should have enough control there (this is ok because there is never a material to deal with in these attach parts)
//also need to bring in the code to do ascii to partdata blob - DONE void ASCIItoPartData(char** output, char* ascii);

// Visibility stuff
// To trigger visibility, add to the model def some animation like this:
//<!--          Pilot's Yoke Animation/GUID Name Definitions     -->
//    <Animation name="hide_pilots_yoke" guid="54647385-cff7-679c-a657-9b9cd33e4f12" length="100" type="Sim" typeParam2="hide_pilots_yoke" typeParam="AutoPlay" />
//    <Animation name="yoke_pilot" guid="54577386-cff7-679c-a658-ab9cd33e4f13" length="100" type="Sim" typeParam2="yoke_pilot" typeParam="AutoPlay" />
//
//<!--          Pilot's Yoke XML Scripts     -->
//   <PartInfo> 
//	<Name>hide_pilots_yoke</Name> 
//		<MouseRect>
//			<Cursor>Hand</Cursor>
//			<TooltipText>Hide Pilot's Yoke</TooltipText>
//			<CallbackCode>
//				(L:YokePilot,bool) ! (&gt;L:YokePilot,bool)
//			</CallbackCode>
//		</MouseRect>
//   </PartInfo> 
//
//   <PartInfo> 
//   	<Name>yoke_pilot</Name>
//    	<Visibility>
//           	<Parameter>
//      		<Code>(L:YokePilot,bool) 0 &gt; if{ 0 } els{ 1 }</Code>
//           	</Parameter>
//    	</Visibility>
//   </PartInfo> 
//The code we will use is:
//_visibility_[NAME] <?xml version="1.0" encoding="ISO-8859-1" ?> <FSMakeMdlData version="9.0"> <Visibility name="%s"> </Visibility> </FSMakeMdlData>
//_mouserect_[NAME] <?xml version="1.0" encoding="ISO-8859-1" ?> <FSMakeMdlData version="9.0"> <MouseRect name="%s"> </MouseRect> </FSMakeMdlData>
//_nocrash_element <?xml version="1.0" encoding="ISO-8859-1" ?> <FSMakeMdlData version="9.0"> </NoCrash> </FSMakeMdlData>


//new for 2.8:
//template BlendDiffuseByBaseAlpha {
// <A623FA7C-37CB-4d17-B702-854E0DBDB467>
// Boolean  BlendDiffByBaseAlpha;
//}
//
//template BlendDiffuseByInverseSpecularMapAlpha {
// <DAA68529-1C27-4182-9D97-E631A4759EA7>
// Boolean  BlendDiffuseByInvSpecAlpha;
//}
//
//template ForceTextureAddressWrapSetting {
// <046EE84C-7977-4a11-AA2B-C79FF5391EDD>
// Boolean  ForceTextureAddressWrap;
//}
//
//template ForceTextureAddressClampSetting {
// <DB108D57-A3A8-4b76-8CB0-8379CDDEC074>
// Boolean  ForceTextureAddressClamp;
//}
//
//template AlphaData {
// <10DB69F3-E0EE-4fb3-8055-63E539EF5885>
// Boolean  ZTestAlpha;
// FLOAT    AlphaTestValue;
// STRING   AlphaTestFunction; -- "Never", "Less", "Equal", "LessEqual", "Greater", "NotEqual", "GreaterEqual", "Always"
// Boolean  FinalAlphaWrite;
// FLOAT    FinalAlphaWriteValue;
//}

#include "stdafx.h"

#define USE_CONSOLE


#define ATTACHPART_UNKNOWN  0
#define ATTACHPART_BYNAME   1
#define ATTACHPART_BYGUID   2
#define ATTACHPART_EFFECT   3
#define ATTACHPART_PLATFORM 4

#define VERSION_NUMBER "3.00"
#define YEAR	"2026"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include "util.h"

#include "fs10mat.h"

ALLEGRO_CONFIG* global_cfg = NULL;
ModelParams param_cache;
FS10Material material_cache[512];
int num_active_materials = 0;
camera int_camera_cache[512];
camera ext_camera_cache[512];
int num_active_ext_cameras = 0;
int num_active_int_cameras = 0;
int contacts_used = 0;
contact contact_cache[128];
void read_contact_parameters(contact* cache, char* sec);
void read_exit_parameters();
void read_fueltank_parameters();
void read_smoke_parameters();
void read_ballast_parameters();
fueltank tank_cache[11];
smoke smoke_cache[128];
ballast ballast_cache[128];
extern float cache_gearc[128];
global_contact globalcontact;
int attacheffect_name_value = 0;
int attachguid_value = 0;
int attachplatform_value = 0;
char other_buff[1024];

void ASCIItoPartData(char* output, char* ascii)
{            
			int numChars = strlen(ascii);          			
            sprintf(output, "PartData {\n%d;\n", numChars+1);			
			char tempBuff[16];
			int wrapcount = 0;
            for (int k = 0; k < numChars; k++)
            {                
				wrapcount++;
				if (wrapcount > 14)
				{
					sprintf(tempBuff, "%d, \n", ascii[k]);
					wrapcount = 0;
				}
				else
				{
					sprintf(tempBuff, "%d, ", ascii[k]);
				}
                strcat(output, tempBuff);				
            }
            strcat(output, "0;\n}//End PartData\n");            
}

float fabs(float a)
{
	if (a < 0)
		return -a;
	else
		return a;
}

void process_specularity(FILE* in, FILE* out)
{
	char buff[1024];
	char line[1024];
	int matline_count = 0;
	float spec = 0;	
	bool apply_block = false;
	while(1)
	{
		fgets(buff, 1024, in);
		if (feof(in)) return;	

		if (strstr(buff, "FS10Material {"))
		{
			matline_count = 0;	
			apply_block = false;
		}
		else
		{
			if (strlen(buff) > 3)
			{		
				matline_count++;
			}
		}

		if (matline_count == 3)
		{
			//strcpy(line, buff);			
			sscanf(buff, "%f;\n", &spec);
			if (spec > 0)
				apply_block = true;
		}

		if (strstr(buff, "} // FS10Material"))
		{
			if (apply_block)
			{
				fprintf(out, "\tBaseMaterialSpecular {\n\t1;  // AllowBaseMaterialSpecular\n\t}\n");
			}			
		}
		
		fprintf(out, buff);		
	}

}

void complete_gears()
{
	char setstring[1024];
	int type = 1; //wheel
	float x, y, z;
	char section[1024];
	float static_percent = 0.8;
	float compression_ratio, static_compression, max_compression;
	for (int k = 0; k < 128; k++)
	{
		if (!gear_cache[k].used)
			continue;

		sprintf(section, "point.%d", k+contacts_used);
		
		if (gear_cache[k].Class)
			type = gear_cache[k].Class;		

		if (gear_cache[k].static_percent)
		{
			static_percent = gear_cache[k].static_percent / 100.0;
		}		

		max_compression = fabs(gear_cache[k].y - cache_gearc[k]);
		static_compression = max_compression * static_percent;

		if (gear_cache[k].static_compression)
			static_compression = gear_cache[k].static_compression;

		compression_ratio = max_compression/static_compression;

		if (gear_cache[k].max_static_compression_ratio)
			compression_ratio = gear_cache[k].max_static_compression_ratio;				

		float damping_ratio = 0.8;
		if (gear_cache[k].damping_ratio)
			damping_ratio = gear_cache[k].damping_ratio;

		printf("GEAR %d: max_comp %.3f; stat_comp %.3f; comp_ratio %.3f\n",
			k, max_compression, static_compression, compression_ratio);

		sprintf(setstring, "%d, %f, %f, %f, %f, %d, %f, %f, %f, %f, %f, %f, %f, %d, %f, %f", 
				type,	

				gear_cache[k].z, 
				gear_cache[k].x, 
				gear_cache[k].y,

				gear_cache[k].impact_damage_threshold,	
				gear_cache[k].brake_map,
				gear_cache[k].wheel_radius,
				gear_cache[k].steering_angle,

				static_compression,
				compression_ratio,
				damping_ratio,

				gear_cache[k].extension_time,
				gear_cache[k].retraction_time,
				gear_cache[k].sound_type,
				gear_cache[k].airspeed_limit,
				gear_cache[k].damage_airspeed);
	
		set_config_string("contact_points", section, setstring);
	}
}

void divide_blends(char* buff, char* sb, char* db)
{
	//look for the first "
	int max = strlen(buff);
	int pos = 0;
	while(buff[pos] != '"')
		pos++;
	pos++;

	//copy the first one until the next "
	int spos = 0;
	while(1)
	{
		if (buff[pos] == '"')
			break;

		sb[spos] = buff[pos];
		spos++; pos++;
	}
	sb[spos] = '\0';
	pos++;

	//now look for the next "
	while(buff[pos] != '"')
		pos++;
	pos++;

	//now write the dest in
	spos = 0;
	while(1)
	{
		if (buff[pos] == '"')
			break;

		db[spos] = buff[pos];
		spos++; pos++;
	}
	db[spos] = '\0';
}

void adjust_emissive_strings(char* buff, int* EmissiveModeFLAG)
{	
	
	if (!strcmp(buff, "NULL"))
	{
		*EmissiveModeFLAG = -1;
		return;			
	}	

	if (!strcmp(buff, "AdditiveNightOnly"))		
	{
		*EmissiveModeFLAG = 1;
		return;
	}
	if (!strcmp(buff, "Blend"))
	{
		*EmissiveModeFLAG = 1;
		return;
	}
	if (!strcmp(buff, "MultiplyBlend"))
	{
		*EmissiveModeFLAG = 1;
		return;
	}
	if (!strcmp(buff, "Additive"))
	{
		*EmissiveModeFLAG = 1;
		return;
	}
	if (!strcmp(buff, "AdditiveNightOnlyUserControlled"))
	{
		*EmissiveModeFLAG = 1;
		return;
	}
	if (!strcmp(buff, "BlendUserControlled"))
	{
		*EmissiveModeFLAG = 1;
		return;
	}
	if (!strcmp(buff, "MultiplyBlendUserControlled"))
	{
		*EmissiveModeFLAG = 1;
		return;
	}
	if (!strcmp(buff, "AdditiveUserControlled"))
	{
		*EmissiveModeFLAG = 1;
		return;
	}		
}

void adjust_src_dst_strings(char* buff, int src, int* SrcBlendFLAG, int* DestBlendFLAG)
{
	if (!strcmp(buff, "NULL"))
	{
		if (src)
			*SrcBlendFLAG = -1;
		else
			*DestBlendFLAG = -1;
		return;
	}

	if (!strcmp(buff, "Zero"))
	{
		if (src)
			*SrcBlendFLAG = 1;
		else
			*DestBlendFLAG = 1;
	}

	if (!strcmp(buff, "One"))
	{
		if (src)
			*SrcBlendFLAG = 1;
		else
			*DestBlendFLAG = 1;
	}		

	if (!strcmp(buff, "SrcColor"))
	{
		if (src)
			*SrcBlendFLAG = 1;
		else
			*DestBlendFLAG = 1;
	}

	if (!strcmp(buff, "InvSrcColor"))
	{
		if (src)
			*SrcBlendFLAG = 1;
		else
			*DestBlendFLAG = 1;
	}

	if (!strcmp(buff, "SrcAlpha"))
	{
		if (src)
			*SrcBlendFLAG = 1;
		else
			*DestBlendFLAG = 1;
	}

	if (!strcmp(buff, "InvSrcAlpha"))
	{
		if (src)
			*SrcBlendFLAG = 1;
		else
			*DestBlendFLAG = 1;
	}

	if (!strcmp(buff, "DestAlpha"))
	{
		if (src)
			*SrcBlendFLAG = 1;
		else
			*DestBlendFLAG = 1;
	}

	if (!strcmp(buff, "InvDestAlpha"))
	{
		if (src)
			*SrcBlendFLAG = 1;
		else
			*DestBlendFLAG = 1;
	}

	if (!strcmp(buff, "DestColor"))
	{
		if (src)
			*SrcBlendFLAG = 1;
		else
			*DestBlendFLAG = 1;
	}

	if (!strcmp(buff, "InvDestColor"))
	{
		if (src)
			*SrcBlendFLAG = 1;
		else
			*DestBlendFLAG = 1;
	}

	
}

int find_active_material(FILE* in, FILE* out, int offset)
{
	char buff[1024];
	while(1)
	{
		fgets(buff, 1024, in);
		if (feof(in)) return 0;

		//search for the line which reads "DiffuseTextureFilename {"
		
		if (strstr(buff, "DiffuseTextureFilename"))
		{			
			fprintf(out, buff);
			//found it; now get the filename and see if we have a match
			do
			{
				fgets(buff, 1024, in);
				if (feof(in)) return 0;
			}while(buff[0] == '\n');
			
			fprintf(out, buff);

			if (strstr(buff, material_cache[offset].identifier))
			{
				//found it
				printf("Found material with id '%s'\n", material_cache[offset].identifier, buff);
				do
				{
					fgets(buff, 1024, in);
					if (feof(in)) return 0;
				}while(buff[0] == '\n');
			
				fprintf(out, buff);

				return 1;
			}

		}
		
		fprintf(out, buff);
	}

	return 1;
}

void check_shadowmap_readiness(FILE* in)
{
	//see if we already have the shadowmap stuff in there; if so,
	//remove the request for the tweak
	char buff[1024];
	while(1)
	{
		fgets(buff, 1023, in);
		if (feof(in))
			break;

		if (strstr(buff, "template ShadowMapReady {"))
		{
			param_cache.ShadowMap_temp_ok = 1;
		}
		else
			if (strstr(buff, "ShadowMapReady {"))
			{
				param_cache.ShadowMap_entry_ok = 1;
			}
	}

	fseek(in, SEEK_SET, 0);
}

int is_a_num(char c)
{
	if (c == '0')
		return 1;
	if (c == '1')
		return 1;
	if (c == '2')
		return 1;
	if (c == '3')
		return 1;
	if (c == '4')
		return 1;
	if (c == '5')
		return 1;
	if (c == '6')
		return 1;
	if (c == '7')
		return 1;
	if (c == '8')
		return 1;
	if (c == '9')
		return 1;

	return 0;
}

void sanitize_effect_name(char* input)
{
	//now if we have an _N at the end we remove it
	//Start at the end of the string. If we see a number, keep looking for numbers or
	//the _ char. If we hit a _, then set the \0 there. Otherwise don't do anything.
	int l = strlen(input);
	if (!is_a_num(input[l-1]))
		return;

	for (int k = l-1; k >= 0; k--)
	{
		if (is_a_num(input[k]))
			continue;

		if ((input[k] == '_') || (input[k] == '.'))
		{
			input[k] = '\0';
			return;
		}
	}
}

void get_effect_args_to_attach(char* input, char* output, char* params, int keywordlen)
{
	//we are looking for the part after ": _attachxxxxxx_"
	int colon_pos = -1;
	int keywordpos = -1;
	int found_first_colon = 0;	
	for (int k = 0; k < strlen(input); k++)
	{
		if (input[k] == ':')
		{
			if (found_first_colon == 0)
			{
				colon_pos = k;
				found_first_colon = 1;
			}
		}
		if (input[k] == '_')
		{
			if (colon_pos == k-2)
			{
				keywordpos = k;
				break;
			}
		}		
	}

	if (keywordpos == -1)
	{
		printf("WARNING: bad attach effect part found ('%s' does not contain a legal name)", input);
		output[0] = '\0';
		return;
	}

	//now we have two termination possibilities: 
	//1. we his a space (we have found no params)
	//2. we find a slash / , that means we have params, need to process those
	char buff[1024];
	strcpy(buff, input+keywordpos+keywordlen);
	int pos = strlen(buff);
	int found_params = 0;
	for (int k = 0; k < strlen(buff); k++)
	{
		if (buff[k] == ' ')
		{
			found_params = 0;
			pos = k;
			break;
		}

		if (buff[k] == '/')
		{
			found_params = 1;
			pos = k;
			break;
		}
	}

	strncpy(output, buff, pos);	
	output[pos] = '\0';

	if (found_params == 0)
		return;

	//now deal with the params
	char buffparam[1024];
	strcpy(buffparam, input+keywordpos+keywordlen+pos+1);
	int parampos = strlen(buffparam);
	for (int k = 0; k < strlen(buffparam); k++)
	{
		if (buffparam[k] == ' ')
		{		
			parampos = k;
			break;
		}
	}
	strncpy(params, buffparam, parampos);	
	params[parampos] = '\0';
}
void get_args_to_attach(char* input, char* output, int keywordlen)
{
	//we are looking for the part after ": _attachxxxxxx_"
	int colon_pos = -1;
	int keywordpos = -1;
	int found_first_colon = 0;	
	for (int k = 0; k < strlen(input); k++)
	{
		if (input[k] == ':')
		{
			if (found_first_colon == 0)
			{
				colon_pos = k;
				found_first_colon = 1;
			}
		}
		if (input[k] == '_')
		{
			if (colon_pos == k-2)
			{
				keywordpos = k;
				break;
			}
		}		
	}

	if (keywordpos == -1)
	{
		printf("WARNING: bad attach part found ('%s' does not contain a legal name)", input);
		output[0] = '\0';
		return;
	}

	char buff[1024];
	strcpy(buff, input+keywordpos+keywordlen);
	int pos = strlen(buff);
	for (int k = 0; k < strlen(buff); k++)
	{
		if (buff[k] == ' ')
		{
			pos = k;
			break;
		}
	}

	strncpy(output, buff, pos);	
	output[pos] = '\0';
}

int verify_platform_type(char* name)
{
	char* legal_names[] = {"ASPHALT","BITUMINOUS","BRICK","CONCRETE","CORAL","DIRT","FOREST","GRASS","GRASS_BUMPY","GRAVEL","HARD_TURF","ICE","LONG_GRASS","MACADAM","OIL_TREATED","PLANKS","SAND","SHALE","SHORT_GRASS","SNOW","STEEL_MATS","TARMAC","URBAN","WATER","WRIGHT_FLYER_TRACK"};
	int num_legal_names = 25;
	int retval = 0;
	for (int k = 0; k < num_legal_names; k++)
	{
		if (strcmp(legal_names[k], name) == 0)
		{
			retval = 1;
			break;
		}
	}
	return retval;
}

int move_to_the_end(FILE* in, FILE* out, char* buff)
{	
	//and now we place everything else down, until we hit "}   // end part frm"		
	//alternatively if we see a new part (with "// part: "), then we hit a child; rewind to just before then
	fpos_t last_line_pos;
	int reuse_line = 0;
	while(1)
	{				
		fgets(buff, 1023, in);		
		int printline = 1;
		int dobreak = 0;		
		if (strstr(buff, "}   // end part frm"))
		{			
			dobreak = 1;			
		}
		if (strstr(buff, "// part: "))
		{			
			printline = 0;
			fprintf(out, "//End of parent\n");				
			reuse_line = 1;
			strcpy(other_buff, buff);
			dobreak = 1;			
		}

		if (printline)
			fprintf(out, buff);
		if (dobreak)
			break;
	}

	return reuse_line;
}

int contains_substr(char* line, char* substr)
{
	if (strlen(line) < strlen(substr))
		return 2;
	
	if (strncmp(line, substr, strlen(substr)) == 0)
		return 1;

	return 0;
}

void clean_part_name(char* line, char* cleanline)
{
	//child parts have the phrase "(child of: " in the title - we need to remove that
	//search for the phrase	
	int offset = 0;
	int retval = -1;
	while(1)
	{
		retval = contains_substr(line+offset, "(child of: ");
		if (retval == 2)
			break;

		if (retval == 1)
		{
			//we have a hit - trim this guy and return
			strncpy(cleanline, line, offset);
			return;
		}

		offset++;
	}

	strcpy(cleanline, line);
}

int do_model(char* line, FILE* in, FILE* out)
{		
	int reuse_line = 0;

	//Place the shadowmapready template in the correct spot
	//which is after the GUIDtoName block
	if (param_cache.ShadowMapReady && 
		(!param_cache.ShadowMap_temp_ok) && 
		(strstr(line, "GuidToName {") &&
		(strstr(line, "template"))))
	{
		fprintf(out, line);
		//keep outputting them until we hit the closing brace
		char buff[1024];
		while(1)
		{
			fgets(buff, 1023, in);
			fprintf(out, "%s", buff);
			if (strstr(buff, "}"))
			{				
				break;
			}
		}
		fprintf(out, "\ntemplate ShadowMapReady {\n<2f4f86a9-fe94-4f75-aa1b-299bbd98907b>\nBoolean SMReady;\n}\n");		
		return reuse_line;
	}
	
	//now place the actual entry, after the guidtoname block
	if (param_cache.ShadowMapReady && (!param_cache.ShadowMap_entry_ok) && (strstr(line, "GuidToName {")))
	{
		fprintf(out, line);
		//keep outputting them until we hit the closing brace
		char buff[1024];
		while(1)
		{
			fgets(buff, 1023, in);
			fprintf(out, buff);
			if ((strstr(buff, "}")) && (!strstr(buff, ";")))
			{				
				break;
			}
		}
		fprintf(out, "\nShadowMapReady {\n\t1;\n}\n");
		return reuse_line;
	}
	
	//get rid of the parent name in the part
	char cleanline[1024];
	clean_part_name(line, cleanline);

	//look for the correct names
	int continue_to_look = 1;
	int moved_to_end = 0;

	//THE VISIBILITY TAG
	if (continue_to_look && (strstr(cleanline, ": _visibility_")))
	{		
		char name_to_attach[1024];
		get_args_to_attach(line, name_to_attach, strlen("_visibility_"));
		sanitize_effect_name(name_to_attach);
		char buff[1024];
		continue_to_look = 0;
		
		//put down the line		
		fprintf(out, "%s", line);

		//put down the next line (frame header)
		fgets(buff, 1023, in);
		fprintf(out, buff);

		//now we need to put down the PartData
		char xml_blob[1024];
		sprintf(xml_blob, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?> <FSMakeMdlData version=\"9.0\"> <Visibility name=\"%s\"> </Visibility> </FSMakeMdlData>", name_to_attach);
		
		//now convert the xml blob to the partdata blob
		char partdata_blob[2048];
		ASCIItoPartData(partdata_blob, xml_blob);
				
		fprintf(out, partdata_blob);		

		//MOVE TO END
		reuse_line = move_to_the_end(in, out, buff);
		moved_to_end = 1;	
	}

	//THE MOUSERECT TAG
	if (continue_to_look && (strstr(cleanline, ": _mouserect_")))
	{		
		char name_to_attach[1024];
		get_args_to_attach(line, name_to_attach, strlen("_mouserect_"));	
		sanitize_effect_name(name_to_attach);
		char buff[1024];
		continue_to_look = 0;
		
		//put down the line
		fprintf(out, line);

		//put down the next line (frame header)
		fgets(buff, 1023, in);
		fprintf(out, buff);

		//now we need to put down the PartData
		char xml_blob[1024];
		sprintf(xml_blob, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?> <FSMakeMdlData version=\"9.0\"> <MouseRect name=\"%s\"> </MouseRect> </FSMakeMdlData>", name_to_attach);
		
		//now convert the xml blob to the partdata blob
		char partdata_blob[2048];
		ASCIItoPartData(partdata_blob, xml_blob);
				
		fprintf(out, partdata_blob);		

		//and now we place everything else down, until we hit "}   // end part frm"		
		reuse_line = move_to_the_end(in, out, buff);
		moved_to_end = 1;
	}

	//THE NOCRASH TAG
	if (continue_to_look && (strstr(cleanline, ": _nocrash")))
	{		
		char name_to_attach[1024];
		//get_args_to_attach(line, name_to_attach, strlen("_nocrash"));	
		char buff[1024];
		continue_to_look = 0;
		
		//put down the line
		fprintf(out, line);

		//put down the next line (frame header)
		fgets(buff, 1023, in);
		fprintf(out, buff);

		//now we need to put down the PartData
		char xml_blob[1024];
		sprintf(xml_blob, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?> <FSMakeMdlData version=\"9.0\"> </NoCrash> </FSMakeMdlData>");
		
		//now convert the xml blob to the partdata blob
		char partdata_blob[2048];
		ASCIItoPartData(partdata_blob, xml_blob);
				
		fprintf(out, partdata_blob);		

		//and now we place everything else down, until we hit "}   // end part frm"		
		reuse_line = move_to_the_end(in, out, buff);
		moved_to_end = 1;
	}

	//INSERT THE ATTACHPOINT STUFF HERE
	//_attachbyname_
	//_attachbyguid_
	//_attacheffect_
	//_attachpltfrm_

	//_attachbyname_ - used to attach carrier parts etc
	if (continue_to_look && (strstr(cleanline, ": _attachbyname_")))
	{		
		char name_to_attach[1024];
		get_args_to_attach(line, name_to_attach, strlen("_attachbyname_"));
		sanitize_effect_name(name_to_attach);
		char buff[1024];
		continue_to_look = 0;
		
		//put down the line
		fprintf(out, line);

		//put down the next line (frame header)
		fgets(buff, 1023, in);
		fprintf(out, buff);

		//now we need to put down the PartData
		char xml_blob[1024];
		sprintf(xml_blob, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?> <FSMakeMdlData version=\"9.0\"> <Attachpoint name=\"%s\"> </Attachpoint> </FSMakeMdlData>", name_to_attach);
		
		//now convert the xml blob to the partdata blob
		char partdata_blob[2048];
		ASCIItoPartData(partdata_blob, xml_blob);
				
		fprintf(out, partdata_blob);		

		//MOVE TO THE END
		//and now we place everything else down, until we hit "}   // end part frm"		
		reuse_line = move_to_the_end(in, out, buff);
		moved_to_end = 1;
	}

	//_attachbyguid_ - used to attach library objects
	if (continue_to_look && (strstr(cleanline, ": _attachbyguid_")))
	{		
		char name_to_attach[1024];
		get_args_to_attach(line, name_to_attach, strlen("_attachbyguid_"));
		sanitize_effect_name(name_to_attach);
		char buff[1024];
		continue_to_look = 0;
		
		//put down the line
		fprintf(out, line);

		//put down the next line (frame header)
		fgets(buff, 1023, in);
		fprintf(out, buff);

		//now we need to put down the PartData
		char point_name[256];
		sprintf(point_name, "attachguid_%d", attachguid_value);		
		attachguid_value++;

		char xml_blob[1024];
		sprintf(xml_blob, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?> <FSMakeMdlData version=\"9.0\"> <Attachpoint name=\"%s\"> <AttachedObject> <LibraryObject name=\"%s\" scale=\"1.0\"/> </AttachedObject> </Attachpoint> </FSMakeMdlData>", point_name, name_to_attach);
		
		//now convert the xml blob to the partdata blob
		char partdata_blob[2048];
		ASCIItoPartData(partdata_blob, xml_blob);
				
		fprintf(out, partdata_blob);		

		//MOVE TO THE END
		//and now we place everything else down, until we hit "}   // end part frm"		
		reuse_line = move_to_the_end(in, out, buff);
		moved_to_end = 1;
	}

	//_attachplatform_ - used to attach platforms
	if (continue_to_look && (strstr(cleanline, ": _attachplatform_")))
	{		
		char name_to_attach[1024];		
		get_args_to_attach(line, name_to_attach, strlen("_attachplatform_"));
		sanitize_effect_name(name_to_attach);
		char buff[1024];
		continue_to_look = 0;

		//verfify the platform type
		_strupr(name_to_attach);
		if (verify_platform_type(name_to_attach) == 0)
			printf("WARNING: Invalid platform type '%s' found", name_to_attach);
		
		//put down the line
		fprintf(out, line);

		//put down the next line (frame header)
		fgets(buff, 1023, in);
		fprintf(out, buff);

		//now we need to put down the PartData
		char point_name[256];
		sprintf(point_name, "attachplatform%d", attachplatform_value);		
		attachplatform_value++;

		char xml_blob[1024];
		sprintf(xml_blob, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?> <FSMakeMdlData version=\"9.0\"> <Platform name=\"%s\" surfaceType=\"%s\" > </Platform> </FSMakeMdlData>", point_name, name_to_attach);
		
		//now convert the xml blob to the partdata blob
		char partdata_blob[2048];
		ASCIItoPartData(partdata_blob, xml_blob);
				
		fprintf(out, partdata_blob);		

		//MOVE TO THE END
		//and now we place everything else down, until we hit "}   // end part frm"		
		reuse_line = move_to_the_end(in, out, buff);
		moved_to_end = 1;
	}

	//_attacheffect_ - used to attach effects
	if (continue_to_look && (strstr(cleanline, ": _attacheffect_")))
	{				
		char effect_params[1024];
		//the default value for effect params in case there are none (this is just a dud)
		sprintf(effect_params, "EffectParams");
		char effect_to_attach[1024];
		get_effect_args_to_attach(line, effect_to_attach, effect_params, strlen("_attacheffect_"));			
		sanitize_effect_name(effect_to_attach);
		char buff[1024];
		continue_to_look = 0;
		
		//put down the line
		fprintf(out, line);

		//put down the next line (frame header)
		fgets(buff, 1023, in);
		fprintf(out, buff);

		//now we need to put down the PartData
		char effect_name[256];
		sprintf(effect_name, "attachpt_%s_%d", effect_to_attach, attacheffect_name_value);		
		attacheffect_name_value++;
		char xml_blob[1024];		
		sprintf(xml_blob, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?> <FSMakeMdlData version=\"9.0\"> <Attachpoint name=\"%s\"> <AttachedObject> <Effect effectName=\"%s\" effectParams=\"%s\"/> </AttachedObject> </Attachpoint> </FSMakeMdlData>", effect_name, effect_to_attach, effect_params);
		
		//now convert the xml blob to the partdata blob
		char partdata_blob[2048];
		ASCIItoPartData(partdata_blob, xml_blob);
						
		fprintf(out, partdata_blob);				

		//MOVE TO THE END
		reuse_line = move_to_the_end(in, out, buff);
		moved_to_end = 1;
	}
		
	if (!moved_to_end)
		fprintf(out, line);			

	return reuse_line;
}

void write_fs10_material_bottom_block(int k, FILE* out)
{
			//char diffuse[1024];
			//char specular[1024];
			//char ambient[1024];
			//char emissive[1024];
			//char reflection[1024];
			//char shininess[1024];
			//char bump[1024];
			//char displacement[1024];
			//char detail[1024];
			//char fresnel[1024];		

			if (material_cache[k].diffuse[0])
			{
				fprintf(out, "\tDiffuseTextureFilename {\n\t\"%s\";\n\t}\n", material_cache[k].diffuse);

			}
			if (material_cache[k].specular[0])
			{
				fprintf(out, "\tSpecularTextureFilename {\n\t\"%s\";\n\t}\n", material_cache[k].specular);

			}			
			if (material_cache[k].ambient[0])
			{
				fprintf(out, "\tAmbientTextureFilename {\n\t\"%s\";\n\t}\n", material_cache[k].ambient);

			}
			if (material_cache[k].emissive[0])
			{
				fprintf(out, "\tEmissiveTextureFilename {\n\t\"%s\";\n\t}\n", material_cache[k].emissive);

			}
			if (material_cache[k].reflection[0])
			{
				fprintf(out, "\tReflectionTextureFilename {\n\t\"%s\";\n\t}\n", material_cache[k].reflection);

			}
			if (material_cache[k].shininess[0])
			{
				fprintf(out, "\tShininessTextureFilename {\n\t\"%s\";\n\t}\n", material_cache[k].shininess);

			}
			if (material_cache[k].bump[0])
			{
				fprintf(out, "\tBumpTextureFilename {\n\t\"%s\";\n\t}\n", material_cache[k].bump);

			}
			if (material_cache[k].displacement[0])
			{
				fprintf(out, "\tDisplacementTextureFilename {\n\t\"%s\";\n\t}\n", material_cache[k].displacement);

			}
			if (material_cache[k].detail[0])
			{
				fprintf(out, "\tDetailTextureFilename {\n\t\"%s\";\n\t}\n", material_cache[k].detail);

			}
			if (material_cache[k].fresnel[0])
			{
				fprintf(out, "\tFresnelTextureFilename {\n\t\"%s\";\n\t}\n", material_cache[k].fresnel);

			}		

			//enhanced parameters;
			fprintf(out, "\tEnhancedParameters  {\n");			
			fprintf(out, "      %d;  // Assume vertical normal\n",material_cache[k].ep_assume_vertical_normal);
			fprintf(out, "      %d;  // Z-Write alpha\n",material_cache[k].ep_z_write_alpha);
			fprintf(out, "      %d;  // No Z-Write\n",material_cache[k].ep_no_z_write);
			fprintf(out, "      %d;  // Volume shadow\n",material_cache[k].ep_volume_shadow);
			fprintf(out, "      %d;  // No shadow\n",material_cache[k].ep_no_shadow);
			fprintf(out, "      %d;  // Prelit vertices\n",material_cache[k].ep_prelit_vertices);
			fprintf(out, "    }\n");

			//bloom stuff here
			if (material_cache[k].bloom_allowbloom != -1)
			{
				fprintf(out, "\tAllowBloom {\n%d;\n}\n", material_cache[k].bloom_allowbloom);
			}	

			if (material_cache[k].bloom_specularbloomfloor > -1)
			{
				fprintf(out, "\tSpecularBloomFloor {\n%f;\n}\n", material_cache[k].bloom_specularbloomfloor);
			}		
			
			if (material_cache[k].bloom_ambientlightscale > -1)
			{
				fprintf(out, "\tAmbientLightScale {\n%f;\n}\n", material_cache[k].bloom_ambientlightscale);
			}		

			if (material_cache[k].bloom_bloombycopy || material_cache[k].bloom_bloommodulatingbyalpha)
			{
				fprintf(out, "\tBloomData {\n%d;\n%d;\n}\n", material_cache[k].bloom_bloombycopy, material_cache[k].bloom_bloommodulatingbyalpha);
			}

			 //misc stuff
			if (material_cache[k].misc_doublesided)
			{
				fprintf(out, "\tDoubleSidedMaterial {\n1;\n}\n");
			}
			
			if (material_cache[k].misc_isnnumber)
			{
				fprintf(out, "\tNNumberTexture {\n1;\n}\n");
			}


			//v2.8 stuff
			if (material_cache[k].BlendDiffuseByInverseSpecularMapAlpha)
			{
				fprintf(out, "\tBlendDiffuseByInverseSpecularMapAlpha {\n1;\n}\n");
			}

			if (material_cache[k].BlendDiffuseByBaseAlpha)
			{
				fprintf(out, "\tBlendDiffuseByBaseAlpha {\n1;\n}\n");
			}

			if (material_cache[k].ForceTextureAddressWrapSetting)
			{
				fprintf(out, "\tForceTextureAddressWrapSetting {\n1;\n}\n");
			}

			if (material_cache[k].ForceTextureAddressClamp)
			{
				fprintf(out, "\tForceTextureAddressClamp {\n1;\n}\n");
			}

			//the AlphaData block
			if (
				(material_cache[k].Alpha_ZTestAlpha) ||
				(material_cache[k].Alpha_FinalAlphaWrite) ||
				(material_cache[k].Alpha_AlphaTestValue > -1) ||
				(material_cache[k].Alpha_FinalAlphaWriteValue > -1) ||
				(material_cache[k].Alpha_AlphaTestFunctionFLAG)
				)
			{
				//template AlphaData {
				// <10DB69F3-E0EE-4fb3-8055-63E539EF5885>
				// Boolean  ZTestAlpha;
				// FLOAT    AlphaTestValue;
				// STRING   AlphaTestFunction; -- "Never", "Less", "Equal", "LessEqual", "Greater", "NotEqual", "GreaterEqual", "Always"
				// Boolean  FinalAlphaWrite;
				// FLOAT    FinalAlphaWriteValue;
				fprintf(out, "\tAlphaData {\n\t%d;\n\t%f;\n\t\"%s\";\n\t%d;\n\t%f;\n\t}\n",
					material_cache[k].Alpha_ZTestAlpha,
					((material_cache[k].Alpha_AlphaTestValue < 0)?0:material_cache[k].Alpha_AlphaTestValue),
					((material_cache[k].Alpha_AlphaTestFunctionFLAG)?material_cache[k].Alpha_AlphaTestFunction:"Never"),
					material_cache[k].Alpha_FinalAlphaWrite,
					((material_cache[k].Alpha_FinalAlphaWriteValue < 0)?0:material_cache[k].Alpha_FinalAlphaWriteValue));
			}
}

int matline_count = -1;

int do_material_352(char* line, FILE* in, FILE* out)
{
	char buff[1024];
	int reuse_line = 0;

	//walk all the mats and see which one this is
	for (int k = 0; k < num_active_materials; k++)
	{	
		char tempname[1030];
		sprintf(tempname, "\"%s\"", material_cache[k].identifier);
		if (strstr(buff, tempname))
		{
			//found it
			printf("Found material with id '%s'\n", material_cache[k].identifier);

			//now write out the rest of FS10Mat stuff
			write_fs10_material_bottom_block(k, out);
			fprintf(out, "} // FS10Material\n");
			printf("\tmaterial '%s' processed!\n", material_cache[k].identifier);	
		}
	}
	
	return reuse_line;
}

int do_material_351(char* line, FILE* in, FILE* out)
{
	int reuse_line = 0;	

	if (strstr(line, "DiffuseTextureFilename")	== NULL)
	{
		reuse_line = do_model(line, in, out);
		return reuse_line;
	}

	//it is a material - process it
	fprintf(out, line);

	char buff[1024];

	//found it; now get the filename and see if we have a match
	do
	{
		fgets(buff, 1024, in);
		if (feof(in)) return reuse_line;
	}while(buff[0] == '\n');
			
	fprintf(out, buff);

	//walk all the mats and see which one this is	

	for (int k = 0; k < num_active_materials; k++)
	{
		// v2.8 Bug here!
		// for cases where we match a VC texture and there is no '.bmp' at the end of the name, we may 
		// match incorrectly due to the strstr - eg. $vc043 and $vc0431 will both be hits
		// to prevent this, we encase the name in quotes before the test - because the line we 
		// get in buff will always have the texture name in quotes, this should prevent this issue, as we are 
		// guaranteed to always be looking for the full name
		char tempname[1030];
		sprintf(tempname, "\"%s\"", material_cache[k].identifier);
		if (strstr(buff, tempname))
		{
			//found it
			printf("Found material with id '%s'\n", material_cache[k].identifier);
			do
			{
				fgets(buff, 1024, in);
				if (feof(in)) return reuse_line;
			}while(buff[0] == '\n');
			
			fprintf(out, buff);

			//find the end of the material

			while(1)
			{
				fgets(buff, 1024, in);
				if (feof(in)) return reuse_line;


				// V3 - support for FSDS 3.5.2
				// There is no FS10Material block by default, so we just write out the lot in this case


				//look for diffuse and specular colours
				//they are the two lines immediately beneath the fs10material{ line
				if (strstr(buff, "FS10Material {"))
				{
					matline_count = 0;
					//fprintf(out, "//MATLINE_COUNT reset\n");
				}
				else
					if (strlen(buff) > 3)
					{
						//fprintf(out, "COUNTEDAS %d\n", matline_count);					
						matline_count++;
					}

				if (matline_count == 1)
				{
					//diffuse colour
					if (material_cache[k].diffuse_ok)
					{						
						//write our own values there
						fprintf(out, "\t%.6f; %.6f; %.6f; %.6f;;\n", material_cache[k].diffuse_r, material_cache[k].diffuse_g, material_cache[k].diffuse_b, material_cache[k].diffuse_a);
						continue;
					}
				}
				
				if (matline_count == 2)
				{
					//specular colour
					if (material_cache[k].specular_ok)
					{
						//fprintf(out, "specular '%s'\n", buff);
						fprintf(out, "\t%.6f; %.6f; %.6f;;\n", material_cache[k].specular_r, material_cache[k].specular_g, material_cache[k].specular_b);
						continue;
					}
				}

				if (matline_count == 3)
				{					
					//specular colour
					if (material_cache[k].specular_p > -1)
					{						
						fprintf(out, "\t%.6f;\n", material_cache[k].specular_p);
						continue;
					}
				}

				//look for detail and bump scales
				if (strstr(buff, "// Detail and bump scales"))
				{
					//what are the current values
					float d, b, use_d, use_b;					
					
					char* pch = strtok (buff,"\t ");
					d = atof(pch);
					pch = strtok (NULL,";");
					b = atof(pch);					

					if (material_cache[k].detail_scale > -1)
						use_d = material_cache[k].detail_scale;
					else
						use_d = d;
						
	
					if (material_cache[k].bump_scale > -1)
						use_b = material_cache[k].bump_scale;
					else
						use_b = b;	
					
						//write in the new value
						fprintf(out,"\t%.6f; %.6f;\t// Detail and bump scales\n", use_d, use_b);						

						continue;
				}
				if (strstr(buff, "// Use global environment"))
				{				
					if (material_cache[k].mp_use_global_env > -1)
					{
						fprintf(out,"\t%d;    // Use global environment\n", material_cache[k].mp_use_global_env);
						continue;
					}
				}				
				if (strstr(buff, "// Do not blend env by inv diffuse alpha"))
				{
					if (material_cache[k].mp_blend_env_by_inv_diff_alpha > -1)
					{
						fprintf(out,"\t%d;     // Do not blend env by inv diffuse alpha\n", material_cache[k].mp_blend_env_by_inv_diff_alpha);						
						continue;
					}

				}
				if (strstr(buff, "// Do not blend env by specular map alpha"))
				{
					if (material_cache[k].mp_blend_env_by_specular_alpha > -1)
					{
						fprintf(out,"\t%d;     // Do not blend env by specular map alpha\n", material_cache[k].mp_blend_env_by_specular_alpha);						
						continue;
					}

				}
				if (strstr(buff, "// Fresnel - diffuse = NO, Specular = YES, Env = YES"))
				{
					int fd, fs, fe;					
					sscanf(buff, "\t%d; %d; %d;", &fd, &fs, &fe);
					if (material_cache[k].mp_fresnel_diffuse > -1)
						fd = material_cache[k].mp_fresnel_diffuse;
					if (material_cache[k].mp_fresnel_specular > -1)
						fs = material_cache[k].mp_fresnel_specular;
					if (material_cache[k].mp_fresnel_env > -1)
						fe = material_cache[k].mp_fresnel_env;

					fprintf(out,"\t%d; %d; %d;    //Fresnel - diffuse = NO, Specular = YES, Env = YES\n", 
						fd,
						fs,
						fe);
					continue;
				}
				if (strstr(buff, "// Precipitation - Use = NO, Offset = NO"))
				{
					int pu, po;
					float poo;
					sscanf(buff, "\t%d; %d; %f;", &pu, &po, &poo);
					
					if (material_cache[k].mp_precip_use > -1)
						pu = material_cache[k].mp_precip_use;
					
					if (material_cache[k].mp_precip_offset > -1)
						po = material_cache[k].mp_precip_offset;

					if(material_cache[k].mp_precip_offset_value > -1)
						poo = material_cache[k].mp_precip_offset_value;

					fprintf(out,"\t%d; %d; %.6f;    // Precipitation - Use = NO, Offset = NO\n", 
							pu, po, poo);
					continue;
				}
				if (strstr(buff, "// Specular Map Power Scale"))
				{
					if (material_cache[k].mp_specular_map_power_scale > -1)
					{
						fprintf(out,"\t%.6f;     // Specular Map Power Scale\n", material_cache[k].mp_specular_map_power_scale);						
						continue;
					}
				}
				
				if (strstr(buff, "// Src/Dest blend"))
				{
					char sb[512];
					char db[512];
					
					divide_blends(buff, sb, db);
	
					if (material_cache[k].SrcBlendFLAG > -1)
						strcpy(sb, material_cache[k].SrcBlend);
					if (material_cache[k].DestBlendFLAG > -1)
						strcpy(db, material_cache[k].DestBlend);					

					fprintf(out, "\t\"%s\"; \"%s\";  // Src/Dest blend\n", sb, db);
					continue;
				}

				if (strstr(buff, "// Emissive Mode"))
				{
					if (material_cache[k].EmissiveModeFLAG > -1)
					{
						fprintf(out, "\t\t\"%s\"; // Emissive Mode\n", material_cache[k].EmissiveMode);
						continue;
					}
				}
				//CONTINUE HERE


				//look for the EnvironmentLevelScale							
				if (strstr(buff, "// Scale environment level factor"))
				{
					if (material_cache[k].EnvironmentLevelScale > -1)
					{
						//write in the new value
						fprintf(out,"\t%.6f;\t// Scale environment level factor\n", material_cache[k].EnvironmentLevelScale);
						continue;
					}
					else
					{
						fprintf(out, buff);				
						continue;
					}
				}

				if (strstr(buff, "} // FS10Material"))
				{
					break;
				}
				
				fprintf(out, buff);				
			}

			//now write out the rest of FS10Mat stuff
			write_fs10_material_bottom_block(k, out);


			fprintf(out, "} // FS10Material\n");
			printf("\tmaterial '%s' processed!\n", material_cache[k].identifier);

			return reuse_line;
		}
	}	
	return reuse_line;
}

int do_material(char* line, FILE* in, FILE* out)
{
	if (compat352 == 1)
	{
		return do_material_352(line, in, out);
	}
	else
	{
		return do_material_351(line, in, out);
	}

}

int process_x_file_material(char* input, char* output)
{
	char buff[1024];

	FILE* in = fopen(input, "rt");
	if (!in)
	{
		return (1002);
	}
	FILE* out = fopen(output, "wt");
	if (!out)
	{
		return (1004);
	}

	check_shadowmap_readiness(in);	

	//fprintf(out, "//This file has been processed by FSDSxTweak v1.0\n//(c)2007 Dave Nunez (david.nunez@uct.ac.za)\n\n");

	//now read a line at a time.
	int reuse_line = 0;

	while(1)
	{
		if (reuse_line == 0)
		{
			fgets(buff, 1024, in);
			if (feof(in)) break;
		}
		else
		{
			strcpy(buff, other_buff);
		}
		
		//hand the lines off to the sorter
		reuse_line = do_material(buff, in, out);
	}
	
	fclose(in);
	fclose(out);

	//parse the specularities and add BaseMaterialSpecular block as needed

	copy_file(output, "t3mp0rary_w0rkspac3.twk");
	
	in = fopen("t3mp0rary_w0rkspac3.twk", "rt");
	if (!in)
	{
		return (1002);
	}
	out = fopen(output, "wt");
	if (!out)
	{
		return (1004);
	}

	process_specularity(in, out);

	remove("t3mp0rary_w0rkspac3.twk");	

	return 0;
}

int process_drp_model(char* input, char* output)
{	
	return 0;

	/*
	char buff[1024];

	FILE* in = fopen(input, "rt");
	if (!in)
	{
		return (1002);
	}
	
	if (!exists(output))
	{
		return (1005);
	}

	push_config_state();
	set_config_file(output);

	//now read a line at a time.
	while(1)
	{
		fgets(buff, 1024, in);
		if (feof(in)) break;
		
		if (strstr(buff, ": _datum"))
		{	
			process_datum_reference_point(in);
			break;
		}
	}
	
	fclose(in);
	pop_config_state();
	return 0;
	*/
}

int process_x_file_model(char* input, char* output)
{	
	char buff[1024];

	FILE* in = fopen(input, "rt");
	if (!in)
	{
		return (1002);
	}
	
	if (!exists(output))
	{
		return (1005);
	}
	
	set_config_file(output);

	//now read a line at a time.
	while(1)
	{
		fgets(buff, 1024, in);
		if (feof(in)) break;
		
		//hand the lines off to the sorter
		do_aircraft_cfg(buff, in);
	}
	
	if (gears_used)
	{
		complete_gears();
	}

	process_globalcontact();

	int newcontactpoints = contacts_used+gears_used;
	
	if (globalcontact.max_number_of_points != -1)
	{
		if (globalcontact.max_number_of_points > newcontactpoints)
			newcontactpoints = globalcontact.max_number_of_points;
	}

	if(contacts_used || gears_used)
		set_config_int("contact_points", "max_number_of_points", newcontactpoints);	

	if(stations_used)
		set_config_int("weight_and_balance", "max_number_of_stations", stations_used);
	if (exits_used)
		set_config_int("exits", "number_of_exits", exits_used);
	
	fclose(in);	
	return 0;
}

void test_decomp();
int compat352 = 0;

int main(int argc, char** argv)
{
	last_config_examined[0] = '\0';
	char result_string[256];	
	al_init();	

	if (argc < 2)
	{
		char buff[512];
		sprintf(buff, "FSDSxTweak v%s\n(c)%s Rabbit Engineering\n\nTweaks various aspects of FSX .x files for use with FSDS (but will work for gmax too!)\n\nUsage:\nTWEAK MODE: fsdsxtweak <KFG file to apply> <.x file to process> <new .x file to output> [aircraft.cfg file to modify]\nSee fsdsxtweak.pdf file for config file setup.\n\nAUDIT MODE: fsdsxtweak /audit <.X file to audit> <new text file to output>\nSee fsdsxtweak.pdf for the output format.", VERSION_NUMBER, YEAR);
		allegro_message(buff);
		return 0;
	}
	
	printf("FSDSxTweak v%s\n(c)%s Rabbit Engineering\n", VERSION_NUMBER, YEAR);

	if (stricmp(argv[1], "/version") == 0)
	{
		//dump the version
		printf("Writing version info to '%s'\n", argv[2]);
		FILE* out = fopen(argv[2], "wt");
		fprintf(out, "%s\n", VERSION_NUMBER);
		fclose(out);
		return 0;
	}

	if (stricmp(argv[1], "/audit") == 0)
	{
		//audit mode
		if (!exists(argv[2]))
		{	
			sprintf(result_string, "Error 1005: Could not open .x file '%s' to audit.\nAbnormal termination. No output written", argv[2]);
			allegro_message(result_string);
			return 0;
		}
		
		int result = audit_x_file(argv[2]);
		result = produce_map_histo(argv[2], argv[3]);
		switch(result)
		{
		case 1006:
			sprintf(result_string, "Error 1006: Could not open file '%s' for writing.\nAbnormal termination. No output written", argv[3]);
			allegro_message(result_string);
			return 0;
			break;		
		}
		printf("File audit completed.\n");
		return 0;
	}
	
	//here we add the /352 compat switch. It must be the first switch in the list.
	int argOffset = 0;	

	if (stricmp(argv[1], "/352") == 0)
	{
		argOffset = 1;
		compat352 = 1;
	}

	if (!exists(argv[1+argOffset]))
	{	
	sprintf(result_string, "Error 1001: Could not open config file '%s' for reading.\nAbnormal termination. No output written", argv[1+argOffset]);
		allegro_message(result_string);
		return 0;
	}

	set_config_file(argv[1+argOffset]);
	srand(clock());
	read_texture_parameters();	
	read_globalcontact();
	read_contact_parameters(contact_cache, "contact");
	read_contact_parameters(gear_cache, "gear");
	read_station_parameters();
	read_exit_parameters();
	read_fueltank_parameters();
	read_smoke_parameters();
	read_ballast_parameters();

	int result = process_x_file_material(argv[2+argOffset], argv[3+argOffset]);
	
	switch(result)
	{
	case 1002:
		sprintf(result_string, "Error 1002: Could not open .x file '%s' for reading.\nAbnormal termination. No output written", argv[2+argOffset]);
		allegro_message(result_string);
		return 0;
		break;
	case 1004:
		sprintf(result_string, "Error 1004: Could not open output .x file '%s' for writing.\nAbnormal termination. No output written", argv[3+argOffset]);
		allegro_message(result_string);
		return 0;
		break;
	}

	printf("Completed all model tweaks.\n");
		
	//do the rest only if we have been passed and aircraft.cfg file
	if (argc > 4+argOffset)
	{
		get_eyepoint(argv[4+argOffset]);		
		preload_camera_configs(ext_camera_cache, "camera", argv[1+argOffset]);
		preload_camera_configs(int_camera_cache, "vccamera", argv[1+argOffset]);		
		int has_model_parameters = read_model_parameters();
		if (has_model_parameters)
		{
			section_spaces_hack_forward(argv[4+argOffset], "t3mp0rary_w0rkspac3.twk");
			copy_file("t3mp0rary_w0rkspac3.twk", argv[4+argOffset]);
			remove("t3mp0rary_w0rkspac3.twk");

//			result = process_drp_model(argv[2], argv[4+argOffset]);
			result = process_x_file_model(argv[2+argOffset], argv[4+argOffset]);			

			// deal with an error case (restult != 0)
			if (result != 0)
			{
				printf("Error in process_x_file_model() [error code:%d]", result);
				return 0;
			}

			//hack the ref speeds
			section_spaces_hack_backward(argv[4+argOffset], "t3mp0rary_w0rkspac3.twk");
			copy_file("t3mp0rary_w0rkspac3.twk", argv[4+argOffset]);
			remove("t3mp0rary_w0rkspac3.twk");
		}	
	}
	return 0;
}



