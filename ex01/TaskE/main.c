#include <stdlib.h>
#include <stdio.h>

#include "array.h"

int main(int argc, char* argv[])
{
	Array array = array_new(10);
	unsigned char i;

	while(1)
	{
		array_insertBack(&array, i++);
	}

	return 0;
}
