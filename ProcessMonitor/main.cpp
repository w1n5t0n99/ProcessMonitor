#include <iostream>

#include "ProcessTracker.h"
#include "AppWrapper.h"


int main()
{
	
	// StartApp - requires full path specified to start it
	// CloseApp - keyword, closes any process with given keyword
//	AppWrapper::StartAppAndFoceClose("C:\\Program Files (x86)\\Respondus\\LockDown Browser Lab\\LockDownBrowserLab.exe", "tvnserver", false);
//	AppWrapper::StartAppOnClose("C:\\Program Files\\TightVNC\\tvnserver.exe", "LockDownBrowserLab", true);

//	AppWrapper::StartAppAndFoceClose("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe", "firefox", false);
//	AppWrapper::StartAppOnClose("C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe", "chrome", false);

	if (ProcessTracker::IsCurrentProcessAdmin())
		std::cout << "ADMIN!" << std::endl;

	std::cin.get();
	return 0;
}
