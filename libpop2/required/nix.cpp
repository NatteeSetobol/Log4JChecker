#include "nix.h"
void* PlatformAllocate(int size)
{
	void* result = malloc(size);

	return result;
}

void PlatformFree(void* chunk)
{
	free(chunk);
	chunk = NULL;
}

void ClearMemory(void* ptr, int size)
{
	memset(ptr, 0, size);
}

char *ReadFile(char* filename)
{
	char *result = NULL;
	FILE *file = NULL;
	int filesize = 0;

	file = fopen(filename, "rb");

	if (file)
	{
		fseek(file,	0,SEEK_END);
		filesize = ftell(file);
		rewind(file);
		
		result = (char*) malloc(filesize+1);
		memset(result,0,filesize-1);
		fread(result,1, filesize,file);

	}

	return result;
}
