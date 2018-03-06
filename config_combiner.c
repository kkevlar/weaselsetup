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

#define LEN_MYBUF 512

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

    char my_buf[LEN_MYBUF];
    FILE* primary_file = fopen(primary_file_name,STRING_READ_MODE);
    FILE* secondary_file = fopen(secondary_file_name,STRING_READ_MODE);
    FILE* outfile = fopen(output_file_name,STRING_WRITE_MODE);

    if (!primary_file ||
        !secondary_file ||
        !outfile)
        combiner_error("Failed to find a file.");

    #warning "Primary copying is disabled"
    printf("\n");
    // move_config_across_files(my_buf, 
    //     primary_file, 
    //     outfile, 
    //     primary_device_number, 
    //     COPY_MODE_PRIMARY);

    printf("\n");
    move_config_across_files(my_buf, 
        secondary_file, 
        outfile, 
        secondary_device_number, 
        COPY_MODE_SECONDARY);

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
    int move_line_count = 0;
    while(fgets(my_buf, LEN_MYBUF, infile))
    {
        if (mode == COPY_MODE_SECONDARY)
        {
            int source_index = 0;
            int dest_index = LEN_MYBUF/2;
            while (
                my_buf[source_index] != '\n' &&
                my_buf[source_index] != '`' &&
                my_buf[source_index] != '\0'
                )
            {
                my_buf[dest_index] = my_buf[source_index];
                source_index++;
                dest_index++;
            }

            if (my_buf[source_index] == '`')
            {   
                dest_index += 
                sprintf(my_buf+dest_index, 
                    "`DInput/%d/Wireless Gamepad:",
                    intended_device_number);
                source_index++;
                while (
                    my_buf[source_index] != '\n' &&
                    my_buf[source_index] != '\0'
                    )
                {
                    my_buf[dest_index] = my_buf[source_index];
                    source_index++;
                    dest_index++;
                }
            }
            fprintf(outfile, "%s",my_buf+LEN_MYBUF/2);
            move_line_count++;
        }
        else if (mode == COPY_MODE_PRIMARY)
        {
            fprintf(outfile, "%s",my_buf);
            move_line_count++;
        }
        fflush(outfile);    
    }

    printf("Moved %d lines.\n",move_line_count);
}

#ifdef comb_test
#warning "Conbine configurations main is enabled."
int main()
{
    combine_configurations(
        "mp-leftonly.ini",
        "smash-R.ini",
        99,
        88,
        "wcopy.ini");
}
#endif
