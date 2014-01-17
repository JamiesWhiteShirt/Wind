#pragma once

namespace MemUtil
{
	class MiniHeap
	{
	protected:
		char* heap;
		unsigned int size;
		unsigned int pos;
	public:
		MiniHeap(unsigned int size);
		~MiniHeap();

		template<class A>
		A* put(A object)
		{
			if(pos + sizeof(A) <= size)
			{
				A* mempos = (A*)(heap + pos);
				new(mempos) A(object);
				pos += sizeof(A);

				return mempos;
			}
			else
			{
				return nullptr;
			}
		}

		void clear();
	};
};