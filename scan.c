#include <stdio.h>
#include <string.h>

#define GCPAD_PATH "weaseltemplate.ini"

int main(void)
{	
	char my_buf[4000];

	printf(".\n");
	FILE* file = fopen(GCPAD_PATH,"r");
	printf(".\n");

	fscanf(file,"%s",my_buf);
	printf(".\n");

	printf("\n%s\n",my_buf);
	printf(".\n");

	fclose(file);   
	printf(".\n");
    return 0;
}

