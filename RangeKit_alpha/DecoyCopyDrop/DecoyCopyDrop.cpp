#include "stdafx.h"

void DoSomething()
{
	//system("cmd.exe /c echo hello world >> temp.txt");
	
	PROCESS_INFORMATION process_info;
	STARTUPINFO startup_info;

	memset(&process_info, 0, sizeof(process_info));
	memset(&startup_info, 0, sizeof(startup_info));
	
	startup_info.wShowWindow = SW_HIDE;

	CreateProcess(
		L"c:\\windows\\system32\\cmd.exe",
		L"cmd.exe /c echo hello world >> coms.log",
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

void MakeCopyAndExecute()
{
	PROCESS_INFORMATION process_info;
	STARTUPINFO startup_info;

	WCHAR szTmp[MAX_PATH];
	WCHAR szTmpFile[MAX_PATH];
	WCHAR filename[MAX_PATH];

	GetModuleFileName(GetModuleHandle(NULL), filename, sizeof(filename));
	
	GetTempPath(MAX_PATH, szTmp);
	GetTempFileName(szTmp, L"br", 0, szTmpFile);

	CopyFile(filename, szTmpFile, FALSE);
		
	memset(&process_info, 0, sizeof(process_info));
	memset(&startup_info, 0, sizeof(startup_info));

	WCHAR cmdline[MAX_PATH];
	_snwprintf_s(cmdline, MAX_PATH - 2, _TRUNCATE, L"%ws RUN", szTmpFile);


	CreateProcess(
		szTmpFile,
		cmdline,
		NULL,
		NULL,
		TRUE,
		DETACHED_PROCESS,
		NULL,
		szTmp,
		&startup_info,
		&process_info);

	ExitProcess(EXIT_SUCCESS);
}

int wmain(int argc, WCHAR* argv[])
{		
	if (WaitForSingleObject(CreateMutex(NULL, FALSE, L"ThisIsTheMalwareMutex"), 30000) == WAIT_TIMEOUT)
		ExitProcess(EXIT_FAILURE);
	
	if ( (argc == 2) && !wcscmp(argv[1], L"RUN") )
	{

		while (true)
		{		
			DoSomething();
			Sleep(1000);
		}			
    }
	else
	{		
		MakeCopyAndExecute();
	}
	
	return 0;
}

