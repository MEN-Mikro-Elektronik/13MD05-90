#include <stdio.h>
#include "m99_desc.c"

main()
{
	FILE *fp;

	fp = fopen("out", "wb+");
	fwrite(&M99_1, 1, sizeof(M99_1), fp);
	fclose(fp);
	return 0;
}
