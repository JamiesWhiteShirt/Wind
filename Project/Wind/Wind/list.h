template <typename A, int size>
class List
{
private:
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