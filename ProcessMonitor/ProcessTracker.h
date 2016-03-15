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
	static int GetAllRunningProcesses(std::vector<String>& vProcNames, std::vector<DWORD>& vProcIDs);
	static bool FindRunningProcesses(const String& procName, std::vector<String>& vProcNames, std::vector<DWORD>& vProcIDs);

	static bool TerminateProcesses(std::vector<DWORD>& vProcIDs);
	static bool StartProcess(const String& procFullPathName, bool hide = false);

	static bool IsProcessRunning(const String& procKeyword, const DWORD& procID);
	static bool HasProcessEnded(const DWORD& procID, const DWORD& waitTime);
	static bool HasAllProcessesEnded(const std::vector<DWORD>& vProcIDs, const DWORD& waitTime);

};