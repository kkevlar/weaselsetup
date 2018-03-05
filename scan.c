#include <stdio.h>
#include <string.h>

#define GCPAD_PATH "weaseltemplate.ini"

int main(void)
{	
	char my_buf[8000];
	FILE* file = fopen(GCPAD_PATH,"r");

	fgets(my_buf, 10, file);

	int result = strcmp("[Profile]",my_buf);

	sprintf(my_buf, "%s", result == 0 ? "Yay" : "Boo");

	printf("%s\n",my_buf);

	fclose(file);   
    return 0;
}

