#ifndef CONFIG_COMBINER_H
#define CONFIG_CONBINER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void combiner_error(char* errormsg);
void combiner_burn_whitespace(FILE* file);
void combine_configurations(char* primary_file_name,
	char* secondary_file_name,
	int primary_device_number,
	int secondary_device_number,
	char* output_file_name);
void move_config_across_files();
#ifdef comb_test
int main();
#endif
#endif
