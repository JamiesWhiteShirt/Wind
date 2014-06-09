#pragma once

class Random
{
private:
	unsigned int seed;
	unsigned int reEntry;

public:
	Random(unsigned int seed);

	int get();
};