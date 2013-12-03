#include "memutil.h"

using namespace MemUtil;

MiniHeap::MiniHeap(unsigned int size)
	: size(size), heap(new char[size])
{

}

MiniHeap::~MiniHeap()
{
	delete[] heap;
}

void MiniHeap::clear()
{
	pos = 0;
}