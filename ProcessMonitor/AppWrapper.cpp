#include "AppWrapper.h"
#include "ProcessTracker.h"
#include <vector>
#include <iostream>

//===============================================================
// Monitor proccesses and start app when other/s are close 
// Closes any process with given name
//================================================================
bool AppWrapper::StartAppOnClose(const std::string& startAppPath, const std::string& closeAppName, bool hide)
{
	std::vector<DWORD> vProcessIDs;
	std::vector<String> vProcessNames;
	// now we need to track lockdown broswer to see when process is ended
	ProcessTracker::FindRunningProcesses(closeAppName, vProcessNames, vProcessIDs);

	if (vProcessIDs.empty())
	{
		std::cout << "... " << closeAppName << " not running" << std::endl;
		return ProcessTracker::StartProcess(startAppPath, hide);
	}
	else
	{
		//check if processes are fully shutdown since TerminateProcess is asyncronous
		while (!ProcessTracker::HasAllProcessesEnded(vProcessIDs, 1000))
		{
			std::cout << "... Waiting for " << closeAppName << " to exit" << std::endl;
		}

		std::cout << "... " << closeAppName << " exited" << std::endl;
		return ProcessTracker::StartProcess(startAppPath, hide);
	}
}

//=============================================================================
// Force any running process with name to exit then run app
//===============================================================================
bool AppWrapper::StartAppAndFoceClose(const std::string& startAppPath, const std::string& closeAppName, bool hide)
{
	std::vector<DWORD> vProcessIDs;
	std::vector<String> vProcessNames;

	ProcessTracker::FindRunningProcesses(closeAppName, vProcessNames, vProcessIDs);

	if (!vProcessNames.empty() && !vProcessIDs.empty())
	{
		std::cout << "... Closing " << closeAppName << std::endl;
		ProcessTracker::TerminateProcesses(vProcessIDs);
	}
	else
	{
		std::cout << "... " << closeAppName << " exited" << std::endl;
		return ProcessTracker::StartProcess(startAppPath, hide);
	}

	//check if processes are fully shutdown since TerminateProcess is asyncronous
	while (!ProcessTracker::HasAllProcessesEnded(vProcessIDs, 1000))
	{
		std::cout << "... Waiting for " << closeAppName << " to exit" << std::endl;
	}
	
	std::cout << "... " << closeAppName << " exited" << std::endl;
	return ProcessTracker::StartProcess(startAppPath, hide);

}

