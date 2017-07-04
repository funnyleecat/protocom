#pragma once

#ifdef WIN32
#   ifdef PROTCOMM_EXPORTS
#       define PROTCOMM_API __declspec(dllexport)
#   else
#       define PROTCOMM_API __declspec(dllimport)
#   endif //PROTCOMM_EXPORTS
#else
#       define PROTCOMM_API
#endif //WIN32

