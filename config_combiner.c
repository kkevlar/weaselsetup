#include "config_combiner.h"

#define STRING_ATTEMPT_DETAIL "\nAttempting to combine: '%s'(%d) + '%s'(%d) -> %s\n"
#define STRING_READ_MODE "r"
#define STRING_WRITE_MODE "w"
#define STRING_ERROR_MALFORMED_PROFILE "Malformed profile line."
#define STRING_ERROR_MALFORMED_DEVICE "Malformed deviceno line."

#define COPY_MODE_PRIMARY 1
#define COPY_MODE_SECONDARY 2

#define PARSE_PROFILE_LINE "[Profile]"
#define PARSE_DEVICE_LINE "Device = DInput/%d/Wireless Gamepad"
#define PARSE_PROFILE_LINE_LENGTH 10

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

    printf("\n");
    move_config_across_files(my_buf, 
        primary_file, 
        outfile, 
        primary_device_number, 
        COPY_MODE_PRIMARY);

    fclose(primary_file);   
    fclose(outfile);
}

void move_config_across_files(char* my_buf, FILE* infile, FILE* outfile, int intended_device_number, int mode)
{
    char modename[16];
    if (COPY_MODE_PRIMARY == mode)
        sprintf(modename,"%s","Primary");
    else if (COPY_MODE_SECONDARY == mode)
        sprintf(modename,"%s","Secondary");

    fgets(my_buf, PARSE_PROFILE_LINE_LENGTH, infile);
    int result = strcmp(PARSE_PROFILE_LINE,my_buf);
    if(result)
        combiner_error(STRING_ERROR_MALFORMED_PROFILE);

    combiner_burn_whitespace(infile);

    int deviceno;
    result = fscanf(infile,PARSE_DEVICE_LINE,&deviceno);
    if(result != 1)
        combiner_error(STRING_ERROR_MALFORMED_DEVICE);

    printf("%s Device Number %d -> %d\n",
        modename,
        deviceno,
        intended_device_number);

    if (mode == COPY_MODE_PRIMARY)
    {
        fprintf(outfile, "%s\n",PARSE_PROFILE_LINE);
        fprintf(outfile, 
            "Device = DInput/%d/Wireless Gamepad\n",
            intended_device_number);
        fflush(outfile);
    }

    printf("Moving %s controls to output file...", modename);
    int primary_move_line_count = 0;
    while(fgets(my_buf, 8000, infile))
    {       

        if (mode == COPY_MODE_SECONDARY)
        {
            //do the ` parsing here
            #warning "Quote parsing incomplete"
        }
        else if (mode == COPY_MODE_PRIMARY)
        {
            fprintf(outfile, "%s",my_buf);
            primary_move_line_count++;
        }
        
        
    }

    printf("Moved %d lines.\n",primary_move_line_count);
}

#ifdef comb_test
#warning "Conbine configurations main mode is enabled."
int main()
{
    combine_configurations("weaseltemplate.ini",
        "null",
        0,
        1,
        "wcopy.ini");
}
#endif
