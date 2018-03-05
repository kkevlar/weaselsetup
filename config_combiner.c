#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int combine_configurations(char* primary_file_name,
	char* secondary_file_name,
	int primary_device_number,
	int secondary_device_number,
	char* output_file_name)
{
	printf("\nAttempting to combine: '%s'(%d) + '%s'(%d) -> %s\n",
		primary_file_name,
		primary_device_number,
		secondary_file_name,
		secondary_device_number,
		output_file_name);

	char my_buf[8000];
	FILE* file = fopen(primary_file_name,"r");
	FILE* outfile = fopen(output_file_name,"w");

	fgets(my_buf, 10, file);
	int result = strcmp("[Profile]",my_buf);
	if(result)
		combiner_error("Malformed profile line.");

	combiner_burn_whitespace(file);

	int deviceno;
	result = fscanf(file,"Device = DInput/%d/Wireless Gamepad",&deviceno);
	if(result != 1)
		combiner_error("Malformed deviceno line.");

	printf("Primary Device Number %d -> %d\n",deviceno,primary_device_number);

	fprintf(outfile, "[Profile]\n");
	fprintf(outfile, "Device = DInput/%d/Wireless Gamepad\n",primary_device_number);
	fflush(outfile);
	
	printf("Moving primary controls to output file...");
	int primary_move_line_count = 0;
	while(1)
	{		
		if (fgets(my_buf, 8000, file))
		{
			fprintf(outfile, "%s",my_buf);
			primary_move_line_count++;
		}
		else
			break;
	}

	printf("Moved %d lines.\n",primary_move_line_count);

	fclose(file);   
	fclose(outfile);
	return 0;
}


int main()
{
	combine_configurations("weaseltemplate.ini",
		"null",
		0,
		1,
		"wcopy.ini");
}

