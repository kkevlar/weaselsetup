#ifndef CONFIG_COMBINER_H
#define CONFIG_CONBINER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int combiner_error(char* errormsg);
void combiner_burn_whitespace(FILE* file);
int combine_configurations(char* primary_file_name,
    char* secondary_file_name,
    #ifndef COMBINER_EXPANDED_DEVICE_ABILITIES
	    int primary_device_number,
	    int secondary_device_number,
	#else
	    char primary_device_name,
	    char secondary_device_name,
	#endif
    char* output_file_name);
int move_config_across_files();
#ifdef comb_test
int main();
#endif
#endif
