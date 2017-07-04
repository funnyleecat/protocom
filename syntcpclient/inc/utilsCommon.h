#pragma once
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../../../../lib/cross/zlib1/x64/DynamicDebug/zlib.lib")
#else
#pragma comment(lib, "../../../../lib/cross/zlib1/x64/DynamicRelease/zlib.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "../../../../lib/cross/zlib1/x86/DynamicDebug/zlib.lib")
#else
#pragma comment(lib, "../../../../lib/cross/zlib1/x86/DynamicRelease/zlib.lib")
#endif
#endif
