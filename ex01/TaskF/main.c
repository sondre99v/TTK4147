#include <stdlib.h>
#include <stdio.h>

#include "array.h"

int main(int argc, char* argv[])
{
	Array array = array_new(10);
	
	for(int i = 0; i < 10000; i++)
	{
		array_insertBack(&array, i);
	}

	return 0;
}
