#include "stdafx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include "util.h"

struct audit_record
{
	char texture[1024];
	int count;

	audit_record() {texture[0] = '\0'; count = 0;};
};

audit_record audit_cache[512];
int audit_cache_size = 0;

void add_audit_record(char* map)
{	
	
	strcpy(audit_cache[audit_cache_size].texture, map);
	audit_cache_size++;	
}

void sanitize_map(char* filename)
{	
	char temp[1024];
	int t = 0;
	char mode = 0;
	for (int k = 0; k < strlen(filename); k++)
	{
		if (mode)
		{
			if (filename[k] == '"')
				continue;
			if (filename[k] == ';')
				continue;
			if (filename[k] == '\r')
				continue;
			if (filename[k] == '\n')
				continue;

			temp[t] = filename[k];
			t++;
		}
		else
		{
			if (filename[k] == '"')
			{
				mode = 1;
			}
		}
	}
	
	temp[t] = '\0';
	strcpy(filename, temp);
}

void process_texture_name(char* name)
{
	//search for it
	for (int k = 0; k < audit_cache_size; k++)
	{
		if (stricmp(name, audit_cache[k].texture) ==0)
		{
			audit_cache[k].count++;
			return;
		}
	}
	add_audit_record(name);
}

int audit_x_file(char* filename)
{
	//this builds a histogram of all the textures used in the FS10Materials in the given x file
	
	FILE* input = fopen(filename, "rt");
	
	char buff[1024];
	
	char DiffuseTextureFilename[1024];
	char SpecularTextureFilename[1024];
	char AmbientTextureFilename[1024];
	char EmissiveTextureFilename[1024];
	char ReflectionTextureFilename[1024];
	char ShininessTextureFilename[1024];
	char BumpTextureFilename[1024];
	char DisplacementTextureFilename[1024];
	char DetailTextureFilename[1024];
	char FresnelTextureFilename[1024];
	
	int complete = 0;

	int scanmode = 0;
	//scan modes:
	//0 = looking for the fs10mat entry in the header
	//1 = looking for the next fs10mat
	//2 = processing the current fs10mat

	while(!complete)
	{
		fgets(buff, 1024, input);
		if (feof(input))
			complete = 1;

		switch(scanmode)
		{
		case 0:
			if (strstr(buff, "template FS10Material {"))
				scanmode = 1;
			break;
		case 1:
			if (strstr(buff, "FS10Material {"))
			{
				scanmode = 2;
				DiffuseTextureFilename[0] = 0;
				SpecularTextureFilename[0] = 0;
				AmbientTextureFilename[0] = 0;
				EmissiveTextureFilename[0] = 0;
				ReflectionTextureFilename[0] = 0;
				ShininessTextureFilename[0] = 0;
				BumpTextureFilename[0] = 0;
				DisplacementTextureFilename[0] = 0;
				DetailTextureFilename[0] = 0;
				FresnelTextureFilename[0] = 0;
			}
			break;
		case 2:
			if (strstr(buff, "DiffuseTextureFilename {"))			
				fgets(DiffuseTextureFilename, 1024, input);

			if (strstr(buff, "SpecularTextureFilename {"))
				fgets(SpecularTextureFilename, 1024, input);

			if (strstr(buff, "AmbientTextureFilename {"))
				fgets(AmbientTextureFilename, 1024, input);

			if (strstr(buff, "EmissiveTextureFilename {"))
				fgets(EmissiveTextureFilename, 1024, input);

			if (strstr(buff, "ReflectionTextureFilename {"))
				fgets(ReflectionTextureFilename, 1024, input);

			if (strstr(buff, "ShininessTextureFilename {"))
				fgets(ShininessTextureFilename, 1024, input);

			if (strstr(buff, "BumpTextureFilename {"))
				fgets(BumpTextureFilename, 1024, input);

			if (strstr(buff, "DisplacementTextureFilename {"))
				fgets(DisplacementTextureFilename, 1024, input);

			if (strstr(buff, "DetailTextureFilename {"))
				fgets(DetailTextureFilename, 1024, input);

			if (strstr(buff, "FresnelTextureFilename {"))
				fgets(FresnelTextureFilename, 1024, input);


			if (strstr(buff, "} // FS10Material"))
			{
				//set this material
				scanmode = 1;
				
				if (DiffuseTextureFilename[0])
				{					
					sanitize_map(DiffuseTextureFilename);
					process_texture_name(DiffuseTextureFilename);
				}
				if (SpecularTextureFilename[0])
				{					
					sanitize_map(SpecularTextureFilename);
					process_texture_name(SpecularTextureFilename);
				}
				if (AmbientTextureFilename[0])
				{					
					sanitize_map(AmbientTextureFilename);
					process_texture_name(AmbientTextureFilename);
				}
				if (EmissiveTextureFilename[0])
				{					
					sanitize_map(EmissiveTextureFilename);
					process_texture_name(EmissiveTextureFilename);
				}
				if (ReflectionTextureFilename[0])
				{					
					sanitize_map(ReflectionTextureFilename);
					process_texture_name(ReflectionTextureFilename);
				}
				if (ShininessTextureFilename[0])
				{					
					sanitize_map(ShininessTextureFilename);
					process_texture_name(ShininessTextureFilename);
				}				
				if (BumpTextureFilename[0])
				{					
					sanitize_map(BumpTextureFilename);
					process_texture_name(BumpTextureFilename);
				}				
				if (DisplacementTextureFilename[0])
				{					
					sanitize_map(DisplacementTextureFilename);
					process_texture_name(DisplacementTextureFilename);
				}				
				if (DetailTextureFilename[0])
				{					
					sanitize_map(DetailTextureFilename);
					process_texture_name(DetailTextureFilename);
				}				
				if (FresnelTextureFilename[0])
				{					
					sanitize_map(FresnelTextureFilename);
					process_texture_name(FresnelTextureFilename);
				}																															
			}
			break;
		}

	}

	fclose(input);
	return 0;
}

int produce_map_histo(char* input, char* _output)
{
	FILE* output = 	fopen(_output, "wt");
	if (!output)
		return 1006;

	fprintf(output, "//The file '%s' uses %d maps:\n", input, audit_cache_size);
	for (int k = 0; k < audit_cache_size; k++)
	{
		fprintf(output, "%s\n", audit_cache[k].texture);
	}
	float sum = 0;
	for (int k = 0; k < audit_cache_size; k++)
		sum += (float)(audit_cache[k].count);

	fprintf(output, "\n//Number of references per texture (average %.2f refs/map):\n", sum/(float)(audit_cache_size));
	for (int k = 0; k < audit_cache_size; k++)
	{
		fprintf(output, "% 5d:%s\n", audit_cache[k].count, audit_cache[k].texture);
	}

	fprintf(output, "\n//PERF WARNING: Bottom 2.5%% of maps by reference (consider consolidating some of these into a single map):\n");
	float threshold = 0.025*sum;
	int numwarnings = 0;
	for (int k = 0; k < audit_cache_size; k++)
	{
		if ((float)audit_cache[k].count <= threshold)
		{
			fprintf(output, "% 5d:%s\n", audit_cache[k].count, audit_cache[k].texture);
			numwarnings++;
		}
	}
	if (numwarnings == 0)
	{
		fprintf(output, "No warning flags on this model.\n");
	}

	fclose(output);
	return 0;
}