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
		content[count++] = object;
	}

	int size()
	{
		return count;
	}

	void clear()
	{
		count = 0;
	}
};