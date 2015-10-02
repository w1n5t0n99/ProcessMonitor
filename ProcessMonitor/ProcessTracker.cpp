#include "ProcessTracker.h"

#include <Psapi.h>
#include <tchar.h>
#include <stdio.h>

//=================================================
// Populate a vector with all running processes
//====================================================
bool ProcessTracker::GetAllRunningProcesses(std::vector<String>& vProcNames, std::vector<DWORD>& vProcIDs)
{
	//clear containers
	vProcIDs.clear();
	vProcNames.clear();

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return false;
	// determine how many processes returned
	cProcesses = cbNeeded / sizeof(DWORD);


	unsigned int i;
	for (i = 0; i < cProcesses; ++i)
	{
		if (aProcesses[i] != 0)
		{
			DWORD processID = aProcesses[i];

			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
			// Get a handle to the process.
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, processID);

			//Open process and get name
			if (NULL != hProcess)
			{
				HMODULE hMod;
				DWORD cbNeeded;
				LPWSTR processPath;

				GetProcessImageFileName(hProcess, szProcessName, _countof(szProcessName));

				//add process data to container
				vProcNames.push_back(String(szProcessName));
				vProcIDs.push_back(processID);
			}

			// Release the handle to the process.
			CloseHandle(hProcess);
		}

	}
	for (int i = 0; i < vProcNames.size(); ++i)
		printf("%s %d\n", vProcNames[i].c_str(), vProcIDs[i]);

	return true;

}

//============================================================================
// Find all processes that match a given name
//============================================================================
bool ProcessTracker::FindRunningProcesses(const String& procName, std::vector<String>& vProcNames, std::vector<DWORD>& vProcIDs)
{
	//clear containers
	vProcIDs.clear();
	vProcNames.clear();

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return false;
	// determine how many processes returned
	cProcesses = cbNeeded / sizeof(DWORD);


	unsigned int i;
	for (i = 0; i < cProcesses; ++i)
	{
		if (aProcesses[i] != 0)
		{
			DWORD processID = aProcesses[i];

			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
			// Get a handle to the process.
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, processID);

			//Open process and get name
			if (NULL != hProcess)
			{
				HMODULE hMod;
				DWORD cbNeeded;
				LPWSTR processPath;

				GetProcessImageFileName(hProcess, szProcessName, _countof(szProcessName));

				String strProcessName(szProcessName);
				if (strProcessName.find(procName) != std::string::npos)
				{
					//add process data to container
					vProcNames.push_back(String(szProcessName));
					vProcIDs.push_back(processID);
				}

			}

			// Release the handle to the process.
			CloseHandle(hProcess);
		}

	}
	for (int i = 0; i < vProcNames.size(); ++i)
		printf("%s %d\n", vProcNames[i].c_str(), vProcIDs[i]);

	return true;
}

//==========================================================
// Terminate processes by process ID
//==========================================================
bool ProcessTracker::TerminateProcesses(std::vector<DWORD>& vProcIDs)
{
	for (unsigned int i = 0; i < vProcIDs.size(); ++i)
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,
			FALSE, vProcIDs[i]);

		DWORD dwExitCode = 0;
		TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

		GetProcessImageFileName(hProcess, szProcessName, _countof(szProcessName));
		String strProcessName(szProcessName);

		GetExitCodeProcess(hProcess, &dwExitCode);
		TerminateProcess(hProcess, dwExitCode);

		printf("Process Terminated: %s Exit Code: %d\n", strProcessName.c_str(), dwExitCode);

		// Release the handle to the process.
		CloseHandle(hProcess);
	}

	return true;
}

//================================================================
// Create process by given path
//===============================================================
bool ProcessTracker::StartProcess(const String& procFullPathName)
{
	HINSTANCE hInstance;

	hInstance = ShellExecute(NULL, NULL, procFullPathName.c_str(), NULL, NULL, SW_HIDE);
	if ((int)hInstance < 32)
	{
		printf("Process not started - %s\n", procFullPathName.c_str());
		return false;
	}
	else
	{
		printf("Process started - %s\n", procFullPathName.c_str());
		return true;
	}

}

//=====================================================================
// Checks if the process with given id is running
//=====================================================================
bool ProcessTracker::GetProcessesStatus(const String& procKeyword, const std::vector<DWORD>& vProcIDs)
{
	bool queryStatus = false;
	for (unsigned int i = 0; i < vProcIDs.size(); ++i)
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, vProcIDs[i]);

		if (NULL == hProcess)
		{
			printf("process not found\n");

		}
		TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
		GetProcessImageFileName(hProcess, szProcessName, _countof(szProcessName));
		String strProcName(szProcessName);

		DWORD dwExitCode = 0;
		GetExitCodeProcess(hProcess, &dwExitCode);
	//	printf("%s - %d\n", strProcName.c_str(), dwExitCode);
		if(dwExitCode == STILL_ACTIVE && strProcName.find(procKeyword) != std::string::npos)
			//at least one of the given PIDs is still active
			queryStatus = true;
	}

	return queryStatus;
}

//=========================================================
// check for given amount of time to see if process ends
//=========================================================
bool ProcessTracker::IsProcessRunning(const DWORD& procID, const DWORD& waitTime)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, procID);

	if (NULL == hProcess)
	{
		printf("process not found\n");
		return false;
	}

	DWORD procStatus = WaitForSingleObject(hProcess, waitTime);
	printf("%ul\n", procStatus);
	if (procStatus == WAIT_TIMEOUT)
	{
		CloseHandle(hProcess);
		return true;
	}
	else if (procStatus == WAIT_OBJECT_0)
	{
		CloseHandle(hProcess);
		return false;
	}
	else if(procStatus == WAIT_FAILED)
	{
		CloseHandle(hProcess);
		return false;
	}
	else 
	{
		CloseHandle(hProcess);
		return false;
	}
	
}

