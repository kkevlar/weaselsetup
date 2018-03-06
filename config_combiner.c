#include "config_combiner.h"

#define STRING_ATTEMPT_DETAIL "\nAttempting to combine: '%s'(%d) + '%s'(%d) -> %s\n"
#define STRING_READ_MODE "r"
#define STRING_WRITE_MODE "w"
#define STRING_ERROR_MALFORMED_PROFILE "Malformed profile line."
#define STRING_ERROR_MALFORMED_DEVICE "Malformed deviceno line."

#define PARSE_PROFILE_LINE "[Profile]"
#define PARSE_DEVICE_LINE "Device = DInput/%d/Wireless Gamepad"

void combiner_error(char* errormsg)
{
	printf("ERROR: %s\n",errormsg);
	printf("Parsing failed.\n");
	exit(1);
}

void combiner_burn_whitespace(FILE* file)
{
	while(1)
	{
		char c[10];
		fgets(c, 2, file);
		if (c[0] == '\n')
			break;
	}
}

void combine_configurations(char* primary_file_name,
	char* secondary_file_name,
	int primary_device_number,
	int secondary_device_number,
	char* output_file_name)
{
	printf(STRING_ATTEMPT_DETAIL,
		primary_file_name,
		primary_device_number,
		secondary_file_name,
		secondary_device_number,
		output_file_name);

	char my_buf[8000];
	FILE* primary_file = fopen(primary_file_name,STRING_READ_MODE);
	FILE* outfile = fopen(output_file_name,STRING_WRITE_MODE);

	fgets(my_buf, 10, primary_file);
	int result = strcmp(PARSE_PROFILE_LINE,my_buf);
	if(result)
		combiner_error(STRING_ERROR_MALFORMED_PROFILE);

	combiner_burn_whitespace(primary_file);

	int deviceno;
	result = fscanf(primary_file,PARSE_DEVICE_LINE,&deviceno);
	if(result != 1)
		combiner_error(STRING_ERROR_MALFORMED_DEVICE);

	printf("Primary Device Number %d -> %d\n",deviceno,primary_device_number);

	fprintf(outfile, "%s\n",PARSE_PROFILE_LINE);
	fprintf(outfile, "Device = DInput/%d/Wireless Gamepad\n",primary_device_number);
	fflush(outfile);
	
	printf("Moving primary controls to output file...");
	int primary_move_line_count = 0;
	while(1)
	{		
		if (fgets(my_buf, 8000, primary_file))
		{
			fprintf(outfile, "%s",my_buf);
			primary_move_line_count++;
		}
		else
			break;
	}

	printf("Moved %d lines.\n",primary_move_line_count);

	fclose(primary_file);   
	fclose(outfile);
}

void move_config_across_files ()
{
	
}


int main()
{
	combine_configurations("weaseltemplate.ini",
		"null",
		0,
		1,
		"wcopy.ini");
}

