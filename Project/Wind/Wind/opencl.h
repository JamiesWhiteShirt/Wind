#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <mutex>
#include <CL/cl.h>

namespace cl
{
	extern cl_platform_id platform;
	extern cl_device_id device;
	extern cl_context context;

	bool staticInit();
	void unload();

	class CommandQueue
	{
	public:
		cl_command_queue queue;
		std::vector<cl_event> lastEvents;
		bool okay;

		CommandQueue();
		~CommandQueue();

		bool create();
		bool sync();
		void addSyncEvent(cl_event event);
	};

	class Buffer
	{
	public:
		bool okay;
		cl_mem mem;
		size_t size;

		Buffer();
		~Buffer();

		bool create(size_t size, int flags, void* data = nullptr);
		bool write(CommandQueue& queue, size_t offset, size_t cb, const void* data);
		bool write(CommandQueue& queue, const void* data);
		bool read(CommandQueue& queue, size_t offset, size_t cb, void* data);
		bool read(CommandQueue& queue, void* data);
		bool copyTo(CommandQueue& queue, Buffer& buffer);
	};

	class Program
	{
	private:
		cl_program program;
		std::map<std::string, cl_kernel> kernels;
		std::mutex mut;
		unsigned int argIndex;
	public:
		cl_kernel preparedKernel;
		bool okay;

		Program();
		Program(std::wstring fileName);
		~Program();

		bool create(std::wstring filename);
		cl_kernel getKernel(std::string kernel);
		bool prepare(std::string kernel);
		bool setArgument(size_t size, const void* value);
		bool setArgument(size_t size, Buffer* value);
		bool invoke(CommandQueue& queue, cl_uint dimensions, const size_t* globalWorkSize, const size_t* localWorkSize);
	};
};