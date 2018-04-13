#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "file.h"

i8 *readFile(const i8 *filename)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL)
	{
		printf("could not load file %s\n", filename);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	u32 length = ftell(file);
	fseek(file, 0, SEEK_SET);
	i8 *buf = malloc(sizeof(i8) * (length + 1));
	fread(buf, 1, length, file);
	buf[length] = '\0';

	fclose(file);

	return buf;
}