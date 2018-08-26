#include "stdafx.h"

// insert signature substrings here in this block to trigger EPP solution
char gszTriggerString[] =
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

// I made this list up - to be fair if you saw these strings in an unknown EXE you would assume it's a RAT -MGH
char gszSuspiciousStrings[] =
" open " \
" download " \
" screenshot " \
" melt " \
" copyfile " \
" upload " \
" delete " \
" keylogger " \
" record " \
" changedns " \
" install " \
" uninstall " \
" stealth " \
" backdoor " \
" rootkit " \
" ";

// must be kept in sync with RangeLib version... OR statically link RangeLib and use existing function
// In this case, to keep the decoy as clean as possible, I just copied the function. -MGH
BOOL GenerateFileName(DWORD key, WCHAR *outFileName)
{
	_snwprintf_s(outFileName, MAX_PATH - 2, _TRUNCATE, L"%ws%08X.bin", L"testfile", key);
	return TRUE;
}

BOOL GenerateLogName(DWORD key, WCHAR *outFileName)
{
	_snwprintf_s(outFileName, MAX_PATH - 2, _TRUNCATE, L"%ws%08X.log", L"coms", key);
	return TRUE;
}

void DoSomething(DWORD key)
{
	//system("cmd.exe /c echo hello world >> temp.txt");
	
	PROCESS_INFORMATION process_info;
	STARTUPINFO startup_info;

	WCHAR szLogname[MAX_PATH];
	WCHAR szCommandLine[MAX_PATH];
	
	GenerateLogName(key, szLogname);

	memset(&process_info, 0, sizeof(process_info));
	memset(&startup_info, 0, sizeof(startup_info));
	
	startup_info.wShowWindow = SW_HIDE;

	_snwprintf_s(szCommandLine, MAX_PATH - 2, _TRUNCATE, L"cmd.exe /c echo hello world >> %ws", szLogname);

	CreateProcess(
		L"c:\\windows\\system32\\cmd.exe",		// TODO use COMSPEC
		szCommandLine,
		NULL,
		NULL,
		TRUE,
		DETACHED_PROCESS,
		NULL,
		NULL,
		&startup_info,
		&process_info);

	CloseHandle(process_info.hThread);
	CloseHandle(process_info.hProcess);
	
	return;
}

// there is no self delete on this
//
void MakeCopyAndExecute(DWORD key)
{
	PROCESS_INFORMATION process_info;
	STARTUPINFO startup_info;

	WCHAR szTmpPath[MAX_PATH];
	WCHAR szTmpFilename[MAX_PATH];
	WCHAR szTmpFullPath[MAX_PATH];
	WCHAR filename[MAX_PATH];

	GetModuleFileName(GetModuleHandle(NULL), filename, sizeof(filename));
	
	GetTempPath(MAX_PATH, szTmpPath);
	//GetTempFileName(szTmp, L"br", 0, szTmpFile); <-- now using generator
	GenerateFileName(key, szTmpFilename);
	_snwprintf_s(szTmpFullPath, MAX_PATH - 2, _TRUNCATE, L"%ws\\%ws", szTmpPath, szTmpFilename);

	CopyFile(filename, szTmpFullPath, FALSE);
		
	memset(&process_info, 0, sizeof(process_info));
	memset(&startup_info, 0, sizeof(startup_info));

	WCHAR cmdline[MAX_PATH];
	_snwprintf_s(cmdline, MAX_PATH - 2, _TRUNCATE, L"%ws %d RUN", szTmpFullPath, key);

	CreateProcess(
		szTmpFilename,
		cmdline,
		NULL,
		NULL,
		TRUE,
		DETACHED_PROCESS,
		NULL,
		szTmpPath,
		&startup_info,
		&process_info);

	ExitProcess(EXIT_SUCCESS);
}

int wmain(int argc, WCHAR* argv[])
{		
	// create mutex to prevent multiple copies
	// if using DEPLOY, the operation should complete in less than 30 seconds 
	// and free the mutex so the secondary EXE can continue and RUN
	if (WaitForSingleObject(CreateMutex(NULL, FALSE, L"ThisIsTheMalwareMutex"), 30000) == WAIT_TIMEOUT)
	{
		wprintf(L"! Only one copy of the decoy can be running at a time.\n");
		ExitProcess(EXIT_FAILURE);
	}
	
	if (3 == argc)
	{
		int seed_value = _wtoi(argv[1]);
		if (0 == seed_value && EINVAL == errno)
		{
			wprintf(L"! When setting the seed you must use a valid integer value.\n");
			exit(EXIT_FAILURE);
		}

		if (!wcscmp(argv[2], L"RUN"))
		{
			while (true)
			{
				DoSomething(seed_value);
				Sleep(1000);
			}
		}
		else
		if (!wcscmp(argv[2], L"DEPLOY"))
		{
			MakeCopyAndExecute(seed_value);
		}
	}
	else
	{
		wprintf(L"Usage:\nDecoyCopyDrop.exe <seed value> [RUN|DEPLOY]\n");
	}

	// force global strings to be present else they will be optimized away
	if (0 == strcmp(gszSuspiciousStrings, "test"))
	{
		wprintf(L"nada");
	}
	if (0 == strcmp(gszTriggerString, "test"))
	{
		wprintf(L"nada");
	}

	return 0;
}

