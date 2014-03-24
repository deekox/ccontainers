#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "avltest.h"

int main(int argc, char *argv[])
{
	srand(time(NULL));
	avltest();
	return 0;
}
