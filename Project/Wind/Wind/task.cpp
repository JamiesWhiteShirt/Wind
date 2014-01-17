#include "task.h"

using namespace Tasks;

bool Task::invoke()
{
	return false;
}

TaskDelete::TaskDelete(void* val)
	: val(val)
{

}

bool TaskDelete::invoke()
{
	delete val;
	return true;
}