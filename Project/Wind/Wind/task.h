#pragma once

#include "list.h"
#include "graphics.h"

namespace Tasks
{
	class Task
	{
	public:
		virtual bool invoke();
	};

	template <typename A>
	class TaskDelete : public Task
	{
	private:
		A* val;
	public:
		TaskDelete(A* val)
			: val(val)
		{

		}

		virtual bool invoke()
		{
			delete val;
			return true;
		}
	};

	class TaskVBOVAOSet : public Task
	{
	private:
		GLuint vbo;
		GLuint vao;
	public:
		TaskVBOVAOSet(GLuint vbo, GLuint vao);
		bool invoke();
	};
};