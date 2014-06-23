#include "opencl.h"
#include "ioutil.h"
#include "window.h"

cl_platform_id cl::platform;
cl_device_id cl::device;
cl_context cl::context;

bool cl::staticInit()
{
	cl_uint num_platforms;
	cl_int error = 0;

	if((error = clGetPlatformIDs(1, &platform, &num_platforms)) != CL_SUCCESS)
	{
		GLWindow::instance->postError("Could not find any OpenCL platforms: " + to_string(error), "OpenCL load error");
		return false;
	}

	if((error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL)) != CL_SUCCESS)
	{
		GLWindow::instance->postError("Could not find a GPU OpenCL device: " + to_string(error), "OpenCL load error");
		return false;
	}

	context = clCreateContext(0, 1, &device, NULL, NULL, &error);
	if(error != CL_SUCCESS)
	{
		GLWindow::instance->postError("Could not create an OpenCL context: " + to_string(error), "OpenCL load error");
		return false;
	}

	return true;
}

void cl::unload()
{
	clReleaseContext(context);
}

cl::CommandQueue::CommandQueue()
	: okay(false)
{

}

cl::CommandQueue::~CommandQueue()
{
	if(okay) clReleaseCommandQueue(queue);
}

bool cl::CommandQueue::create()
{
	cl_int error = 0;
	queue = clCreateCommandQueue(cl::context, cl::device, 0, &error);
	if(error != CL_SUCCESS)
	{
		GLWindow::instance->postError("Could not create command queue: " + to_string(error), "Command queue creation error");
		return false;
	}

	okay = true;
	return true;
}

bool cl::CommandQueue::sync()
{
	cl_int error = clWaitForEvents((cl_uint)lastEvents.size(), &lastEvents[0]);
	if(error != CL_SUCCESS)
	{
		GLWindow::instance->postError("Failed to wait for events to finish: " + to_string(error), "OpenCL sync error");
		return false;
	}

	for(auto iter = lastEvents.begin(); iter != lastEvents.end(); ++iter)
	{
		error = clReleaseEvent(*iter);
		if(error != CL_SUCCESS)
		{
			GLWindow::instance->postError("Failed to release event: " + to_string(error), "OpenCL sync error");
			return false;
		}
	}

	lastEvents.clear();

	return true;
}

void cl::CommandQueue::addSyncEvent(cl_event event)
{
	lastEvents.push_back(event);
}

cl::Program::Program()
	: okay(false)
{

}

cl::Program::Program(std::wstring fileName)
	: okay(false), preparedKernel(0), argIndex(0)
{
	create(fileName);
}

bool cl::Program::create(std::wstring fileName)
{
	cl_int error = 0;

	std::ifstream file(fileName);

	if(!file.good())
	{
		GLWindow::instance->postError("Invalid/missing file", "OpenCL program creation error");
		file.close();
		return false;
	}

	std::string source_str = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	const char* source = source_str.c_str();

	program = clCreateProgramWithSource(context, 1, &source, NULL, &error);
	if(error != CL_SUCCESS)
	{
		GLWindow::instance->postError("Could not create program: " + to_string(error), "OpenCL program creation error");
		return false;
	}

	error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	if(error != CL_SUCCESS)
	{
		size_t log_size;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char* log_msg = new char[log_size + 1];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log_msg, NULL);

		GLWindow::instance->postError(log_msg, "OpenCL program compilation error");

		delete[] log_msg;
		return false;
	}

	okay = true;
	return true;
}

cl::Program::~Program()
{
	if(okay)
	{
		cl_int error;

		for(map<std::string, cl_kernel>::const_iterator iter = kernels.begin(); iter != kernels.end(); ++iter)
		{
			error = clReleaseKernel(iter->second);
			if(error != CL_SUCCESS)
			{
				GLWindow::instance->postError("Could not release kernel: " + to_string(error), "OpenCL program deletion error");
				return;
			}
		}
		error = clReleaseProgram(program);
		if(error != CL_SUCCESS)
		{
			GLWindow::instance->postError("Could not release program: " + to_string(error), "OpenCL program deletion error");
		}
	}
}

cl_kernel cl::Program::getKernel(std::string kernel)
{
	if(!okay) return 0;
	map<std::string, cl_kernel>::iterator it = kernels.find(kernel);
	if(it != kernels.end())
	{
		return it->second;
	}

	cl_int error = 0;
	cl_kernel k = clCreateKernel(program, kernel.c_str(), &error);

	if(error == CL_SUCCESS)
	{
		kernels[kernel] = k;
		return k;
	}

	GLWindow::instance->postError("Tried to use nonexistant kernel: " + to_string(error), "OpenCL kernel error");
	return 0;
}

bool cl::Program::prepare(std::string kernel)
{
	cl_kernel k = getKernel(kernel);

	if(k)
	{
		mut.lock();
		preparedKernel = k;
		argIndex = 0;
		return true;
	}
	else
	{
		GLWindow::instance->postError("Could not prepare kernel for execution", "OpenCL kernel error");
		return false;
	}
}

bool cl::Program::setArgument(size_t size, const void* value)
{
	if(!preparedKernel)
	{
		GLWindow::instance->postError("Tried to set argument for unprepared kernel", "OpenCL kernel argument error");
		return false;
	}
	cl_int error = clSetKernelArg(preparedKernel, argIndex++, size, value);
	if(error != CL_SUCCESS)
	{
		GLWindow::instance->postError("Failed to set kernel argument: " + to_string(error), "OpenCL kernel argument error");
		return false;
	}
	return true;
}

bool cl::Program::setArgument(size_t size, Buffer* value)
{
	return setArgument(size, &value->mem);
}

bool cl::Program::invoke(CommandQueue& queue, cl_uint dimensions, const size_t* globalWorkSize, const size_t* localWorkSize)
{
	if(!preparedKernel)
	{
		GLWindow::instance->postError("Tried to execute unprepared kernel", "OpenCL program execution error");
		return false;
	}

	cl_int error;
	cl_event event = 0;

	error = clEnqueueNDRangeKernel(queue.queue, preparedKernel, dimensions, NULL, globalWorkSize, localWorkSize, 0, NULL, &event);
	if(error != CL_SUCCESS)
	{
		GLWindow::instance->postError("Failed to exectue kernel: " + to_string(error), "OpenCL program execution error");
		return false;
	}

	queue.addSyncEvent(event);

	preparedKernel = 0;
	mut.unlock();

	return true;
}

cl::Buffer::Buffer()
	: okay(false)
{

}

cl::Buffer::~Buffer()
{
	if(okay)
	{
		cl_int error = clReleaseMemObject(mem);
		if(error != CL_SUCCESS)
		{
			GLWindow::instance->postError("Failed to delete buffer: " + to_string(error), "OpenCL buffer deletion error");
		}
	}
}

bool cl::Buffer::create(size_t size, int flags, void* data)
{
	cl_int error;
	mem = clCreateBuffer(context, flags, size, data, &error);
	if(error != CL_SUCCESS)
	{
		GLWindow::instance->postError("Failed to create buffer: " + to_string(error), "OpenCL buffer creation error");
		return false;
	}

	this->size = size;
	okay = true;
	return true;
}

bool cl::Buffer::write(CommandQueue& queue, size_t offset, size_t cb, const void* data)
{
	if(!okay) return false;

	cl_event event = 0;

	cl_int error = clEnqueueWriteBuffer(queue.queue, mem, CL_TRUE, offset, cb, data, 0, NULL, &event);
	if(error != CL_SUCCESS)
	{
		GLWindow::instance->postError("Failed to write to buffer: " + to_string(error), "OpenCL buffer write error");
		return false;
	}

	queue.addSyncEvent(event);

	return true;
}

bool cl::Buffer::write(CommandQueue& queue, const void* data)
{
	return write(queue, 0, size, data);
}

bool cl::Buffer::read(CommandQueue& queue, size_t offset, size_t cb, void* data)
{
	if(!okay) return false;

	cl_event event = 0;

	cl_int error = clEnqueueReadBuffer(queue.queue, mem, CL_TRUE, offset, cb, data, 0, NULL, &event);
	if(error)
	{
		GLWindow::instance->postError("Failed to read buffer: " + to_string(error), "OpenCL buffer read error");
		return false;
	}

	queue.addSyncEvent(event);

	return true;
}

bool cl::Buffer::read(CommandQueue& queue, void* data)
{
	return read(queue, 0, size, data);
}

bool cl::Buffer::copyTo(CommandQueue& queue, Buffer& buffer)
{
	if(!okay) return false;
	
	cl_event event = 0;

	cl_int error = clEnqueueCopyBuffer(queue.queue, mem, buffer.mem, 0, 0, size, 0, nullptr, &event);
	if(error != CL_SUCCESS)
	{
		GLWindow::instance->postError("Failed to copy buffer: " + to_string(error), "OpenCL buffer copy error");
		return false;
	}

	queue.addSyncEvent(event);

	return true;
}