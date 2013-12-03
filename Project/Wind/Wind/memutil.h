namespace MemUtil
{
	class MiniHeap
	{
	private:
		char* heap;
		unsigned int size;
		unsigned int pos;
	public:
		MiniHeap(unsigned int size);
		~MiniHeap();

		template<class A>
		A* put(A object)
		{
			A* mempos = (A*)(heap + pos);
			new(mempos) A(object);
			//memcpy(mempos, &object, sizeof(A));
			pos += sizeof(A);

			return mempos;
		}

		void clear();
	};
};