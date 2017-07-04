#pragma once


#ifdef WIN32
#   ifdef SYNTCPCLIENT_EXPORTS
#       define SYNTCPCLI_API __declspec(dllexport)
#   else
#       define SYNTCPCLI_API __declspec(dllimport)
#   endif //UTILS_EXPORTS
#else
#   define SYNTCPCLI_API
#endif //WIN32
