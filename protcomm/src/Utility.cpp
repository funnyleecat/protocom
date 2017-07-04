#include "stdafx.h"
#include "Utility.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef WIN32
#define __Vsprintf_t vsprintf_s
#else
#define __Vsprintf_t vsnprintf
#endif

namespace protcomm
{
    string Vsprintf_t(const char* format, ...)
    {
        string strReturn;

        int size = 4096;
        char* buffer = new char[size];

        while (1)
        {
            va_list args;
            va_start(args, format);
            int n = __Vsprintf_t(buffer, size, format, args);
            va_end(args);

            if ((n > 0) && (n < size))
            {
                strReturn = buffer;
                delete[] buffer;
                break;
            }
            else
            {
                size *= 2;
                delete[] buffer;
                buffer = new char[size];
            }
        }

        return strReturn;
    }

	string GetHexUUIDStr(string const &str)
	{
		if (str.size() < 16)
		{
			return "error-uuid: " + str;
		}
		char szBuf[33] = { 0 };
		szBuf[32] = 0;
		int* pStr = (int*)(str.data());
#ifdef WIN32
		sprintf_s(szBuf, "%8x%8x%8x%8x", *(pStr), *(pStr + 1), *(pStr + 2), *(pStr + 3));
#else
		sprintf(szBuf, "%8x%8x%8x%8x", *(pStr), *(pStr + 1), *(pStr + 2), *(pStr + 3));
#endif // WIN32

		return szBuf;
	}
}
