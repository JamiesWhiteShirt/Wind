#pragma once

#include "memutil.h"

template <typename A, int size>
class List
{
protected:
	A content[size];
	int count;
public:
	List()
	{

	}

	A& operator[](int index)
	{
		return content[index];
	}

	void put(A object)
	{
		if(count < size - 1)
		{
			content[count++] = object;
		}
	}

	int getSize()
	{
		return count;
	}

	void clear()
	{
		count = 0;
	}
};

template <typename A, int maxAmount, unsigned int allocSize>
class VirtualList : protected List<A, maxAmount>, protected MemUtil::MiniHeap
{
public:
	VirtualList()
		: MemUtil::MiniHeap(allocSize)
	{

	}

	~VirtualList()
	{
		MemUtil::MiniHeap::~MiniHeap();
	}

	A* operator[](int index)
	{
		return List::operator[](index);
	}

	template <typename B>
	A* put(B object)
	{
		A* pObject = MemUtil::MiniHeap::put(object);
		List::put(pObject);
		return pObject;
	}

	int getSize()
	{
		return List::getSize();
	}

	void clear()
	{
		List::clear();
		MemUtil::MiniHeap::clear();
	}
};