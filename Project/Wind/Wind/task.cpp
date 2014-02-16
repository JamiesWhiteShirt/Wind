#include "task.h"

using namespace Tasks;

bool Task::invoke()
{
	return false;
}


TaskVBOVAOSet::TaskVBOVAOSet(GLuint vbo, GLuint vao)
	: vbo(vbo), vao(vao)
{

}

bool TaskVBOVAOSet::invoke()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	return true;
}