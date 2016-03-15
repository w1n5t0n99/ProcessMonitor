#include <iostream>

#include "ProcessTracker.h"
#include "AppWrapper.h"


int main()
{
	
	// StartApp - requires full path specified to start it
	// CloseApp - keyword, closes any process with given keyword
	AppWrapper::StartAppAndFoceClose("C:\\Program Files (x86)\\Respondus\\LockDown Browser Lab\\LockDownBrowserLab.exe", "tvnserver.exe", false);
	AppWrapper::StartAppOnClose("C:\\Program Files\\TightVNC\\tvnserver.exe", "LockDownBrowserLab", true);

	//std::cin.get();
	return 0;
}