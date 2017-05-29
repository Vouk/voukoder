#include "ExporterX264Common.h"

void copyConvertStringLiteralIntoUTF16(const wchar_t* inputString, prUTF16Char* destination)
{
#ifdef PRMAC_ENV
	int length = wcslen(inputString);
	CFRange	range = { 0, kPrMaxPath };
	range.length = length;
	CFStringRef inputStringCFSR = CFStringCreateWithBytes(kCFAllocatorDefault, reinterpret_cast<const UInt8 *>(inputString), length * sizeof(wchar_t), kCFStringEncodingUTF32LE, kPrFalse);
	CFStringGetBytes(inputStringCFSR, range, kCFStringEncodingUTF16, 0, kPrFalse, reinterpret_cast<UInt8 *>(destination), length * (sizeof(prUTF16Char)), NULL);
	destination[length] = 0; // Set NULL-terminator, since CFString calls don't set it, and MediaCore hosts expect it
	CFRelease(inputStringCFSR);
#elif defined PRWIN_ENV
	size_t length = wcslen(inputString);
	wcscpy_s(destination, length + 1, inputString);
#endif
}

void safeStrCpy(char *destStr, int size, const char *srcStr)
{
#ifdef PRWIN_ENV
	strcpy_s(destStr, size, srcStr);
#elif defined PRMAC_ENV
	strcpy(destStr, srcStr);
#endif
}