#pragma once

#include "EncoderInfo.h"

// Types from nvcuda.dll
typedef int(*__cuDeviceGetCount)(int *count);
typedef int(*__cuDeviceGetName)(char *name, int len, int dev);
typedef int(*__cuDeviceComputeCapability)(int *major, int *minor, int dev);

class NvidiaCustomOptions
{
public:
	static bool GetOptions(EncoderInfo &encoderInfo);
};