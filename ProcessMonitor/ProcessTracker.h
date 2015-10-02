#pragma once

#include <vector>
#include <string>
#include <Windows.h>

#ifndef UNICODE
	typedef std::string String;
#else
	typedef std::wstring String;
#endif



class ProcessTracker
{
public:
	bool GetAllRunningProcesses(std::vector<String>& vProcNames, std::vector<DWORD>& vProcIDs);
	bool FindRunningProcesses(const String& procName, std::vector<String>& vProcNames, std::vector<DWORD>& vProcIDs);

	bool TerminateProcesses(std::vector<DWORD>& vProcIDs);
	bool StartProcess(const String& procFullPathName);

	bool GetProcessesStatus(const String& procKeyword, const std::vector<DWORD>& vProcIDs);
	bool IsProcessRunning(const DWORD& procID, const DWORD& waitTime);
};