#include "globalutil.h"
#include <stdlib.h>

Random::Random(unsigned int seed)
	: seed(seed), reEntry(0)
{

}

int Random::get()
{
	srand(seed);
	return rand_r(&reEntry);
}