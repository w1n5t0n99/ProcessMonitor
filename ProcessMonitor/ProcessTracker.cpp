#include "ProcessTracker.h"

#include <iostream>
#include <algorithm>

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <tchar.h>
#include <stdio.h>

//=================================================
// Populate a vector with all running processes
//====================================================
int ProcessTracker::GetAllRunningProcesses(std::vector<String>& vProcNames, std::vector<DWORD>& vProcIDs)
{
	//clear containers
	vProcIDs.clear();
	vProcNames.clear();

	DWORD aProcesses[1024], cbNeeded, cProcesses;

	// Retrieves the process identifier for each process object in system
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return false;
	// determine how many processes returned
	cProcesses = cbNeeded / sizeof(DWORD);

	// Iterate through all found processes
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

#ifdef _DEBUG
	for (int i = 0; i < vProcNames.size(); ++i)
		printf("%s %d\n", vProcNames[i].c_str(), vProcIDs[i]);
#endif

	// return number of processes found
	return vProcNames.size();

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

#ifdef _DEBUG
	for (int i = 0; i < vProcNames.size(); ++i)
		printf("%s %d\n", vProcNames[i].c_str(), vProcIDs[i]);
#endif

	return true;
}

//==========================================================
// Terminate processes by process ID
//==========================================================
bool ProcessTracker::TerminateProcesses(std::vector<DWORD>& vProcIDs)
{
	for (unsigned int i = 0; i < vProcIDs.size(); ++i)
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,
			FALSE, vProcIDs[i]);

		DWORD dwExitCode = 0;
		TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

		GetProcessImageFileName(hProcess, szProcessName, _countof(szProcessName));
		String strProcessName(szProcessName);

		GetExitCodeProcess(hProcess, &dwExitCode);
		TerminateProcess(hProcess, dwExitCode);
		
#ifdef _DEBUG
		printf("Process Terminated: %s Exit Code: %d\n", strProcessName.c_str(), dwExitCode);
#endif

		// Release the handle to the process.
		CloseHandle(hProcess);
	}

	return true;
}

//================================================================
// Create process by given path
//===============================================================
bool ProcessTracker::StartProcess(const String& procFullPathName, bool hide)
{
	HINSTANCE hInstance;

	if(hide)
		hInstance = ShellExecute(NULL, NULL, procFullPathName.c_str(), NULL, NULL, SW_HIDE);
	else
		hInstance = ShellExecute(NULL, NULL, procFullPathName.c_str(), NULL, NULL, SW_SHOWNORMAL);


	if ((int)hInstance < 32)
	{
#ifdef _DEBUG
		printf("Process not started - %s\n", procFullPathName.c_str());
#endif
		return false;
	}
	else
	{
#ifdef _DEBUG
		printf("Process started - %s\n", procFullPathName.c_str());
#endif
		return true;
	}

}

//=====================================================================
// Checks if the process with given id is running
//=====================================================================
bool ProcessTracker::IsProcessRunning(const String& procKeyword, const DWORD& procID)
{

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ, FALSE, procID);

	if (NULL == hProcess)
	{
#ifdef _DEBUG
		printf("process not found\n");
#endif


		return false;
	}

	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	GetProcessImageFileName(hProcess, szProcessName, _countof(szProcessName));
	String strProcName(szProcessName);

	DWORD dwExitCode = 0;
	GetExitCodeProcess(hProcess, &dwExitCode);
	// printf("%s - %d\n", strProcName.c_str(), dwExitCode);
	if (dwExitCode == STILL_ACTIVE && strProcName.find(procKeyword) != std::string::npos)
		//the pID is still active
		return true;
	else
		return false;
}

//===================================================================
// check for given amount of time in ms to see if process ends
//=====================================================================
bool ProcessTracker::HasProcessEnded(const DWORD& procID, const DWORD& waitTime)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, procID);

	if (NULL == hProcess)
	{
		printf("process not found\n");
		return true;
	}

	DWORD procStatus = WaitForSingleObject(hProcess, waitTime);

	if (procStatus == WAIT_TIMEOUT)
	{
		CloseHandle(hProcess);
		return false;
	}
	else if (procStatus == WAIT_OBJECT_0)
	{
		CloseHandle(hProcess);
		return true;
	}
	else if(procStatus == WAIT_FAILED)
	{
		CloseHandle(hProcess);
		return true;
	}
	else 
	{
		CloseHandle(hProcess);
		return true;
	}
	
}

//====================================================================
// Check if all processes have ended, if any active returns false
//===================================================================
bool ProcessTracker::HasAllProcessesEnded(const std::vector<DWORD>& vProcIDs, const DWORD& waitTime)
{
	// Get handles for all process IDs
	std::vector<HANDLE> vHandles;

	for (unsigned int i = 0; i < vProcIDs.size(); ++i)
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, vProcIDs[i]);
		
		if (NULL != hProcess)
			vHandles.push_back(hProcess);

	}

	//Vectors stored contiguously
	HANDLE* arrHandle = &vHandles[0];
	DWORD procStatus = WaitForMultipleObjects(vHandles.size(), arrHandle, true, waitTime);

	if (procStatus == WAIT_TIMEOUT)
	{
		for (unsigned int i = 0; i < vHandles.size(); ++i)
			CloseHandle(vHandles[i]);
		return false;
	}
	else if (procStatus == WAIT_OBJECT_0)
	{
		for (unsigned int i = 0; i < vHandles.size(); ++i)
			CloseHandle(vHandles[i]);
 		return true;
	}
	else if (procStatus == WAIT_FAILED)
	{
		for (unsigned int i = 0; i < vHandles.size(); ++i)
			CloseHandle(vHandles[i]);
		return true;
	}
	else
	{
		for (unsigned int i = 0; i < vHandles.size(); ++i)
			CloseHandle(vHandles[i]);
		return true;
	}
}

//==============================================================================
// Check if all processes have ended using CreateToolHelp32Snapshot
//===============================================================================
int ProcessTracker::HasAllProcessesEnded_Snapshot(const std::vector<DWORD>& vProcIDs, const DWORD& waitTime)
{
	//==========================================================
	HANDLE hProcess, hProcessSnap;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	//Wait before checking processes
	Sleep(waitTime);

	//Take snapshot of all processes in system
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "CreateToolHelp32 INVALID_HANDLE_VALUE" << std::endl;
		return -1;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		std::cout << "Process32First Error" << std::endl;
		CloseHandle(hProcessSnap);
		return -1;

	}

	//Check first returned process
	//std::cout << pe32.szExeFile << " | " << pe32.th32ProcessID << std::endl;
	if (std::find(vProcIDs.begin(), vProcIDs.end(), pe32.th32ParentProcessID) != vProcIDs.end())
		return 0;


	while (Process32Next(hProcessSnap, &pe32))
	{
	//	std::cout << pe32.szExeFile << " | " << pe32.th32ProcessID << std::endl
		if (std::find(vProcIDs.begin(), vProcIDs.end(), pe32.th32ParentProcessID) != vProcIDs.end())
			return 0;
	}

	CloseHandle(hProcessSnap);
	return 1;


}

//==========================================================
// Check if account for current process has admin rights
//==============================================================
bool ProcessTracker::IsCurrentProcessAdmin()
{
	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup; 
	b = AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0, &AdministratorsGroup); 

	if(b) 
	{
	 if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
	 {
         b = FALSE;
	 } 
    FreeSid(AdministratorsGroup); 
}

	return(b);
}


