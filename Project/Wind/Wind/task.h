#pragma once

#include "list.h"

namespace Tasks
{
	class Task
	{
	public:
		virtual bool invoke();
	};

	class TaskDelete : public Task
	{
	private:
		void* val;
	public:
		TaskDelete(void* val);
		virtual bool invoke();
	};
};