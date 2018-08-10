#include "Pipe.h"
#include <atlconv.h>
#include <vector>

Pipe::Pipe()
{}

Pipe::~Pipe()
{}

bool Pipe::open(const wstring commandLine)
{
	// Set the bInheritHandle flag so pipe handles are inherited. 
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe for the child process's STDIN. 
	CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0);

	// Ensure the write handle to the pipe for STDIN is not inherited. 
	SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

	std::vector<wchar_t> vec(commandLine.begin(), commandLine.end());
	vec.push_back(L'\0');

	// Set up members of the PROCESS_INFORMATION structure. 
	PROCESS_INFORMATION piProcInfo;
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.
	STARTUPINFO siStartInfo;
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process
	BOOL bSuccess = CreateProcess(NULL, &vec[0], NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);
	if (!bSuccess)
	{
		av_log(NULL, AV_LOG_ERROR, "Can't allocate buffer\n");
		return false;
	}

	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);
}

void Pipe::close()
{
	CloseHandle(g_hChildStd_IN_Wr);
	CloseHandle(g_hChildStd_IN_Rd);
}

bool Pipe::write(const AVFrame *frame)
{
	const AVPixelFormat format = (AVPixelFormat)frame->format;
	const int bufferSize = av_image_get_buffer_size(format, frame->width, frame->height, 16);
	
	// Reserve a buffer for the uncompressed frame data
	uint8_t *buffer = (uint8_t*)av_malloc(bufferSize);
	if (!buffer)
	{
	    av_log(NULL, AV_LOG_ERROR, "Can't allocate buffer\n");
		return false;
	}

	bool ret = true;

	// Fill the buffer and send it to the child process
	if (av_image_copy_to_buffer(buffer, bufferSize, (const uint8_t* const *)frame->data, (const int*)frame->linesize, format, frame->width, frame->height, 1) > 0)
	{
		DWORD dwWritten = 0;
		ret = WriteFile(g_hChildStd_IN_Wr, buffer, bufferSize, &dwWritten, NULL);
	}

	av_free(buffer);

	return ret;
}
