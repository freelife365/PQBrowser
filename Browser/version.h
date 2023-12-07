#pragma once

#define _VERSIONTOSTRING(arg) #arg
#define VERSIONTOSTRING(arg)  _VERSIONTOSTRING(arg)

#define VERSION_MAJOR         0
#define VERSION_MINOR         1
#define VERSION_PATCH         1
#define VERSION_BUILD         5
#define VERSION_SOURCE        4515a6b
#define VERSION_PRERELEASE    alpha
#define VERSION_FULL          VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_BUILD
#define STR_VERSION_FILE      VERSIONTOSTRING(VERSION_MAJOR.VERSION_MINOR.VERSION_PATCH)
#define STR_VERSION_PRODUCT   VERSIONTOSTRING(VERSION_MAJOR.VERSION_MINOR.VERSION_PATCH(VERSION_SOURCE))                                                                   
