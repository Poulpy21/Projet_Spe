
#include "clUtils.hpp"
#include <fstream>

using namespace log4cpp;

namespace utils {
	
	void clAssert(cl_int err, const std::string &file, int line, bool abort) {
		if (err != CL_SUCCESS)
		{
			log4cpp::log_console->errorStream() << "OpenCL assert false :\n\t\t" 
				<< openCLGetErrorString(err) << " in file " <<  file << ":" << line << ".";

			if (abort) 
				exit(EXIT_FAILURE);
		}
	}

	void loadDevicesAndCreateContexts(
			std::vector<cl::Platform> &platforms,
			std::vector<cl::Context> &gpuContexts, 
			std::vector<cl::Context> &cpuContexts, 
			std::vector<cl::Context> &accContexts, 
			std::vector<std::vector<cl::Device>> &gpuDevices,
			std::vector<std::vector<cl::Device>> &cpuDevices,
			std::vector<std::vector<cl::Device>> &accDevices,
			unsigned int &nPlatforms,
			unsigned int &nTotGpuDevices, 
			unsigned int &nTotCpuDevices, 
			unsigned int &nTotAccDevices,
			unsigned int **nGpuDevices,
			unsigned int **nCpuDevices,
			unsigned int **nAccDevices
			) 

	{
		nTotGpuDevices = 0;
		nTotCpuDevices = 0;
		nTotAccDevices = 0;

		CHK_ERROR_RET(cl::Platform::get(&platforms));
		nPlatforms = platforms.size();

		*nGpuDevices = new unsigned int[nPlatforms];
		*nCpuDevices = new unsigned int[nPlatforms];
		*nAccDevices = new unsigned int[nPlatforms];

		log_console->infoStream() << "\tFound " << nPlatforms << " platforms.";

		//Check devices and create contexts
		log_console->infoStream() << "Listing devices..";
		unsigned int i_gpu=0, i_cpu=0, i_acc=0;
		cl_int err;
		for (auto it = platforms.begin(); it != platforms.end(); ++it) {

			std::string name;
			it->getInfo(CL_PLATFORM_NAME, &name);
			log_console->infoStream() << "Switching to platform " << name;

			struct ContextUserData **userData = new struct ContextUserData*[3];
			for (int i = 0; i < 3; i++) userData[i] = new struct ContextUserData;
			userData[0]->platform=&(*it); userData[0]->deviceType=CL_DEVICE_TYPE_GPU; userData[0]->contextId=i_gpu;
			userData[1]->platform=&(*it); userData[1]->deviceType=CL_DEVICE_TYPE_CPU; userData[1]->contextId=i_cpu;
			userData[2]->platform=&(*it); userData[2]->deviceType=CL_DEVICE_TYPE_ACCELERATOR; userData[2]->contextId=i_acc;
	
			cl_context_properties contextProperties[3] = { 
				CL_CONTEXT_PLATFORM, (cl_context_properties)(*it)(), 
				0 
			};

			cl::Context gpu_context(CL_DEVICE_TYPE_GPU, contextProperties, openclContextCallback, (void*)userData[0], &err);
			if(err != CL_DEVICE_NOT_FOUND && err != CL_INVALID_PROPERTY) {
				CHK_ERRORS(err);
				std::vector<cl::Device> gpu_devices = gpu_context.getInfo<CL_CONTEXT_DEVICES>(&err);
				CHK_ERRORS(err);
				if(gpu_devices.size() != 0) {
					log_console->infoStream() << "\tFound " << gpu_devices.size() << " GPUs.";
					nTotGpuDevices += gpu_devices.size();
					(*nGpuDevices)[i_gpu] = gpu_devices.size();
					gpuDevices.push_back(gpu_devices);
					gpuContexts.push_back(gpu_context);
					i_gpu++;
				}
			}
	

				cl::Context cpu_context(CL_DEVICE_TYPE_CPU, contextProperties, openclContextCallback, (void*)userData[1], &err);
				if(err != CL_DEVICE_NOT_FOUND) {
					CHK_ERRORS(err);
					std::vector<cl::Device> cpu_devices = cpu_context.getInfo<CL_CONTEXT_DEVICES>(&err);
					CHK_ERRORS(err);
					if(cpu_devices.size() != 0) {
						log_console->infoStream() << "\tFound " << cpu_devices.size() << " CPUs.";
						nTotCpuDevices += cpu_devices.size();
						(*nCpuDevices)[i_cpu] = cpu_devices.size();
						cpuDevices.push_back(cpu_devices);
						cpuContexts.push_back(cpu_context);
						i_cpu++;
					}
				}

				cl::Context acc_context(CL_DEVICE_TYPE_ACCELERATOR, contextProperties, openclContextCallback, (void*)userData[2], &err);
				if(err != CL_DEVICE_NOT_FOUND) {
					CHK_ERRORS(err);

					std::vector<cl::Device> acc_devices = acc_context.getInfo<CL_CONTEXT_DEVICES>(&err);
					CHK_ERRORS(err);
					if(acc_devices.size() != 0) {
						log_console->infoStream() << "\tFound " << acc_devices.size() << " accelerator devices.";
						nTotAccDevices += acc_devices.size();
						(*nAccDevices)[i_acc] = acc_devices.size();
						accDevices.push_back(acc_devices);
						accContexts.push_back(acc_context);
						i_acc++;
					}
				}
		}	
	}

	cl::Program::Sources loadSourcesFromFile(const std::string &srcFile) {
		log_console->infoStream() << "Loading sources from file '" << srcFile << "'.";
		std::ifstream sourceFile(srcFile);

		if(!sourceFile.good()) {
			log_console->errorStream() << "Error while loading sources from file '" << srcFile << "'.";
			exit(EXIT_FAILURE);
		}

		sourceFile.seekg(0,std::ios::end);
		unsigned int length = sourceFile.tellg();
		sourceFile.seekg(0,std::ios::beg);
		char *buffer = new char[length+1];
		sourceFile.read(buffer,length);
		buffer[length] = '\0';

		cl::Program::Sources sources(1, std::make_pair(buffer, length+1));

		return sources;
	}


	void CL_CALLBACK openclContextCallback(const char *errorInfo, const void *privateInfoSize, size_t cb, void *userData) {
		struct ContextUserData *data = (struct ContextUserData *) userData;

		log_console->debugStream() << "Context Call Back";

		std::string platformName;
		data->platform->getInfo(CL_PLATFORM_NAME, &platformName);

		log_console->errorStream() << "OpenCL context callback :\n\t\t"
			<< "Context " << " (platform=" << platformName 
			<< ",device type="<<toStringDeviceType(data->deviceType)
			<< ",id=" << data->contextId
			<< ")\n\t\t" << errorInfo;

		exit(EXIT_FAILURE);
	}

	void CL_CALLBACK openclBuildCallback(cl_program program, void *user_data) {

		log_console->debugStream() << "Build Call Back";

		struct BuildUserData *userData = (struct BuildUserData*) user_data;
		cl::Program *pgm = userData->program;

		cl_int err;
		std::string src = pgm->getInfo<CL_PROGRAM_SOURCE>(&err); CHK_ERRORS(err);
		std::vector<cl_device_id> devices = pgm->getInfo<CL_PROGRAM_DEVICES>(&err); CHK_ERRORS(err);

		std::vector<std::string> buildLogs, buildOptions;
		std::vector<cl_build_status> buildStatus;

		for (auto it = devices.begin(); it < devices.end(); ++it) {
			buildOptions.push_back(pgm->getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(*it, &err)); CHK_ERRORS(err);
			buildLogs.push_back(pgm->getBuildInfo<CL_PROGRAM_BUILD_LOG>(*it, &err)); CHK_ERRORS(err);
			buildStatus.push_back(pgm->getBuildInfo<CL_PROGRAM_BUILD_STATUS>(*it, &err)); CHK_ERRORS(err);
		}

		bool error = false;

		std::stringstream ss;
		ss << "Building sources from program '" << userData->programName << "' for devices ";
		for (unsigned int i = 0; i < devices.size(); i++) {
			ss << cl::Device(devices[i]).getInfo<CL_DEVICE_NAME>() << " (" << devices[i] << ")";
			if(i!=devices.size()-1) ss << ", ";
			else ss << " :";
		}

		for (unsigned int i = 0; i < devices.size(); i++) {
			ss << "\n\tDevice " << cl::Device(devices[i]).getInfo<CL_DEVICE_NAME>();
			ss << " : " << toStringBuildStatus(buildStatus[i]);

			if(buildStatus[i] != CL_BUILD_SUCCESS) {
				error = true;

				ss << "\n\tBuild options : " << buildOptions[i];
				ss << "\n\tBuild logs : " << buildLogs[i];
				ss << "\n";	
			}
		}

		log_console->infoStream() << ss.str();

		if(error) {
			log_console->errorStream() << "There were errors while building from program '" << userData->programName << "', aborting !";
			exit(EXIT_FAILURE);
		}
	}

	void buildProgram(cl::Program &program, std::vector<cl::Device> &targetDevices, const char *buildOptions, const std::string &programName) {
		struct BuildUserData *data = new struct BuildUserData;
		data->program = &program;
		data->programName = programName;

		CHK_ERROR_RET(program.build(targetDevices, buildOptions, utils::openclBuildCallback, data));
	}

	const std::string toStringDeviceType(cl_device_type deviceType) {
		switch(deviceType) {
			case(CL_DEVICE_TYPE_ALL): return "CL_DEVICE_TYPE_ALL"; break;
			case(CL_DEVICE_TYPE_DEFAULT): return "CL_DEVICE_TYPE_DEFAULT"; break;
			case(CL_DEVICE_TYPE_CPU): return "CL_DEVICE_TYPE_CPU"; break;
			case(CL_DEVICE_TYPE_GPU): return "CL_DEVICE_TYPE_GPU"; break;
			case(CL_DEVICE_TYPE_ACCELERATOR): return "CL_DEVICE_TYPE_ACCELERATOR"; break;
			default: return "Unknown OpenCL Device Type";
		}
	}

	const std::string toStringBuildStatus(cl_build_status buildStatus) {

		switch(buildStatus) {
			case(CL_BUILD_NONE): return "CL_BUILD_NONE"; break;
			case(CL_BUILD_ERROR): return "CL_BUILD_ERROR"; break;
			case(CL_BUILD_SUCCESS): return "CL_BUILD_SUCCESS"; break;
			case(CL_BUILD_IN_PROGRESS): return "CL_BUILD_IN_PROGRESS"; break;
			default: return "Unknown OpenCL Build Status";
		}
	}
}
