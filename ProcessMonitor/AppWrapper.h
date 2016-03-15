#pragma once
#ifndef _APPWRAPPER_
#define _APPWRAPPER_

#include <string>

class AppWrapper
{
public:
	static bool StartAppOnClose(const std::string& startAppPath, const std::string& closeAppName, bool hide = false);
	static bool StartAppAndFoceClose(const std::string& startAppPath, const std::string& closeAppName, bool hide = false);

};

#endif

