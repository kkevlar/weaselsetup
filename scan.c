#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GCPAD_PATH "weaseltemplate.ini"

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

int main(void)
{	
	char my_buf[8000];
	FILE* file = fopen(GCPAD_PATH,"r");

	fgets(my_buf, 10, file);
	int result = strcmp("[Profile]",my_buf);
	if(result)
		boo("Malformed profile line.");

	fresh(file);

	int deviceno;
	result = fscanf(file,"Device = DInput/%d/Wireless Gamepad",&deviceno);
	if(result != 1)
		boo("Malformed deviceno line.");

	fresh(file);

	printf("Deviceno=%d\n",deviceno);



	result = fscanf(file,"%s = `%s`",my_buf, my_buf+4000);

	if(result!= 2)
		boo("Malformed binding line.");


	fclose(file);   
    return 0;
}

