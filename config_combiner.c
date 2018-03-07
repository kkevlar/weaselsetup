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

#define LEN_MYBUF 4000

#if defined(COMB_TEST) || defined(COMB_SHOW_DEBUG_OUTPUT) 
#define DOPRINT 
#endif

int combiner_error(char* errormsg)
{   
    #ifdef DOPRINT
        printf("ERROR: %s\n",errormsg);
        printf("Parsing failed.\n");
    #endif
    return 1;
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
    #ifndef COMBINER_EXPANDED_DEVICE_ABILITIES
        int primary_device_number,
        int secondary_device_number,
    #else
        char primary_device_name,
        char secondary_device_name,
    #endif
    char* output_file_name)
{
    #ifdef DOPRINT
        printf(STRING_ATTEMPT_DETAIL,
            primary_file_name,
            primary_device_number,
            secondary_file_name,
            secondary_device_number,
            output_file_name);
    #endif

    char my_buf[LEN_MYBUF];
    FILE* primary_file = fopen(primary_file_name,STRING_READ_MODE);
    FILE* secondary_file = fopen(secondary_file_name,STRING_READ_MODE);
    FILE* outfile = fopen(output_file_name,STRING_WRITE_MODE);

    int failure = 0;

    if (!primary_file ||
        !secondary_file ||
        !outfile)
        return combiner_error("Failed to find a file.");

    #ifdef COMB_NO_1
        #warning "Primary copying is disabled"
        #ifdef DOPRINT
            printf("\nPrimary copying is disabled.\n");
        #endif
    #else
        failure = move_config_across_files(my_buf, 
            primary_file, 
            outfile, 
            primary_device_number, 
            COPY_MODE_PRIMARY);
        if (failure)
        {
            fclose(primary_file);   
            fclose(secondary_file);   
            fclose(outfile);
            return failure;
        }
    #endif

    #ifdef COMB_NO_2
        #warning "Secondary copying is disabled"
        #ifdef DOPRINT
            printf("\nSecondary copying is disabled\n");
        #endif
    #else
        failure = move_config_across_files(my_buf, 
            secondary_file, 
            outfile, 
            secondary_device_number, 
            COPY_MODE_SECONDARY);
        if (failure)
        {
            fclose(primary_file);   
            fclose(secondary_file);   
            fclose(outfile);
            return failure;
        }
    #endif

    fclose(primary_file);   
    fclose(secondary_file);   
    fclose(outfile);
    return 0;
}

int move_config_across_files(char* my_buf, FILE* infile, FILE* outfile, int intended_device_number, int mode)
{
    // printf("ASS %d\n",intended_device_number);

    #ifdef DOPRINT
        char modename[16];
        if (COPY_MODE_PRIMARY == mode)
            sprintf(modename,"%s","Primary");
        else if (COPY_MODE_SECONDARY == mode)
            sprintf(modename,"%s","Secondary");
    #endif

    fgets(my_buf, PARSE_PROFILE_LINE_LENGTH, infile);
    int result = strcmp(PARSE_PROFILE_LINE,my_buf);
    if(result)
        return combiner_error(STRING_ERROR_MALFORMED_PROFILE);

    combiner_burn_whitespace(infile);

    int deviceno;
    result = fscanf(infile,PARSE_DEVICE_LINE,&deviceno);
    if(result != 1)
        return combiner_error(STRING_ERROR_MALFORMED_DEVICE);

    #ifdef DOPRINT
        printf("\n%s Device Number %d -> %d\n",
            modename,
            deviceno,
            intended_device_number);
    #endif

    if (mode == COPY_MODE_PRIMARY)
    {   
        #ifdef DOPRINT
            printf("Printing combined file header.\n");
        #endif
        fprintf(outfile, "%s\n",PARSE_PROFILE_LINE);
        fprintf(outfile, 
            "Device = DInput/%d/Wireless Gamepad\n",
            intended_device_number);
        fflush(outfile);
    }

    #ifdef DOPRINT
        printf("Moving %s controls to output file...", modename);
    #endif

    int move_line_count = 0;
    while(fgets(my_buf, LEN_MYBUF, infile))
    {
        if (mode == COPY_MODE_SECONDARY)
        {
            int source_index = 0;
            int dest_index = LEN_MYBUF/2;
            my_buf[dest_index+0] = '\0';
            my_buf[dest_index+1] = '\0';
            my_buf[dest_index+2] = '\0';
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
                my_buf[dest_index] = my_buf[source_index];
                source_index++;
                dest_index++;
            }
            fprintf(outfile, "%s",my_buf+(LEN_MYBUF/2));
            move_line_count++;
        }
        else if (mode == COPY_MODE_PRIMARY)
        {
            fprintf(outfile, "%s",my_buf);
            move_line_count++;
        }
        fflush(outfile);    
    }

    #ifdef DOPRINT
        printf("Moved %d lines.\n",move_line_count);
    #endif

    return 0;
}

#ifdef COMB_TEST
#warning "Conbine configurations main is enabled."
int main()
{
    combine_configurations(
        "../../Dolphin Emulator/Config/Profiles/GCPad/k-smash-l.ini",
        "../../Dolphin Emulator/Config/Profiles/GCPad/mp-leftonly.ini",
        7,
        4,
        "../../Dolphin Emulator/Config/Profiles/GCPad/0-comp-test-output.ini");
    return 0;
}
#endif
