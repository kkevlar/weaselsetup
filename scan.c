#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void boo(char* errormsg)
{
	printf("ERROR: %s\n",errormsg);
	printf("Parsing failed.\n");
	exit(1);
}

void fresh(FILE* file)
{
	while(1)
	{
		char c[10];
		fgets(c, 2, file);
		if (c[0] == '\n')
			break;
	}
}

int combine_weasels(char* primary_file_name,
	char* secondary_file_name,
	int primary_device_number,
	int secondary_device_number,
	char* output_file_name)
{
	char my_buf[8000];
	FILE* file = fopen(primary_file_name,"r");
	FILE* outfile = fopen(output_file_name,"w");

	fgets(my_buf, 10, file);
	int result = strcmp("[Profile]",my_buf);
	if(result)
		boo("Malformed profile line.");

	fresh(file);

	int deviceno;
	result = fscanf(file,"Device = DInput/%d/Wireless Gamepad",&deviceno);
	if(result != 1)
		boo("Malformed deviceno line.");

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
	combine_weasels("weaseltemplate.ini",
		"null",
		0,
		1,
		"wcopy.ini");
}
// int main(void)
// {
// 	char my_buf[8000];
// 	FILE* file = fopen(GCPAD_PATH,"r");
// 	FILE* outfile = fopen("wcopy.ini","w");

// 	fgets(my_buf, 10, file);
// 	int result = strcmp("[Profile]",my_buf);
// 	if(result)
// 		boo("Malformed profile line.");

// 	fresh(file);

// 	int deviceno;
// 	result = fscanf(file,"Device = DInput/%d/Wireless Gamepad",&deviceno);
// 	if(result != 1)
// 		boo("Malformed deviceno line.");

// 	printf("Device Number=%d\n",deviceno);

// 	fprintf(outfile, "[Profile]\n");
// 	fprintf(outfile, "Device = DInput/%d/Wireless Gamepad\n",deviceno);
// 	fflush(outfile);
	
// 	while(1)
// 	{		
// 		if (fgets(my_buf, 8000, file))
// 			fprintf(outfile, "%s",my_buf);
// 		else
// 			break;
// 	}

// 	fclose(file);   
// 	fclose(outfile);
// 	return 0;
// }

