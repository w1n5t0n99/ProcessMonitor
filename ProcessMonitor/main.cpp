#include <iostream>
#include <string>
#include <Windows.h>

#include <Psapi.h>
#include <tchar.h>
#include <stdio.h>

#include <vector>

#include "ProcessTracker.h"

int main()
{
	

	std::vector<DWORD> vProcessIDs;
	std::vector<String> vProcessNames;

	ProcessTracker procTracker;
	
	procTracker.FindRunningProcesses("tvnserver.exe", vProcessNames, vProcessIDs);

	if(!vProcessNames.empty())
		procTracker.TerminateProcesses(vProcessIDs);

	//check if processes are fully shutdown since TerminateProcess is asyncronous
	/*
	while (!vProcessNames.empty())
	{
		procTracker.FindRunningProcesses("tvnserver.exe", vProcessNames, vProcessIDs);
		Sleep(1000);
		printf("...waiting for TightVNC shutdown\n");
	}
	*/
	while (procTracker.IsProcessRunning(vProcessIDs[0], 1000))
	{

		printf("...waiting for TightVNC shutdown\n");
	}
	printf("... TightVNC shutdown\n");

	procTracker.StartProcess("C:\\Program Files (x86)\\Respondus LockDown Browser - Lab Version\\LockDownLab.exe");

	// now we need to track lockdown broswer to see when process is ended
	procTracker.FindRunningProcesses("LockDownLab.exe", vProcessNames, vProcessIDs);

	if (vProcessIDs.empty())
		procTracker.StartProcess("C:\\Program Files\\TightVNC\\tvnserver.exe");
	else
	{
		/*
		bool procRunning = procTracker.GetProcessesStatus("LockDownLab", vProcessIDs);
		while (procRunning)
		{
			procRunning = procTracker.GetProcessesStatus("LockDownLab", vProcessIDs);
			//procTracker.FindRunningProcesses("LockDownLab.exe", vProcessNames, vProcessIDs);
			Sleep(2000);
			printf("...waiting for LockDown Browser shutdown\n");
		}
		*/
		while (procTracker.IsProcessRunning(vProcessIDs[0], 1000))
		{

			printf("...waiting for LockDown Browser shutdown\n");
		}
		printf("... LockDown Browser shutdown\n");

		procTracker.StartProcess("C:\\Program Files\\TightVNC\\tvnserver.exe");
	}
	
	


/*

	//procTracker.GetAllRunningProcesses(vProcessNames, vProcessIDs);
	procTracker.FindRunningProcesses("MicrosoftEdge", vProcessNames, vProcessIDs);
	if (!vProcessIDs.empty())
	{
		while (procTracker.IsProcessRunning(vProcessIDs[0], 1000))
		{

			printf("... Process Running\n");
		}
		printf("--- Process Ended\n");
	}
	*/
	std::cin.get();
	return 0;
}