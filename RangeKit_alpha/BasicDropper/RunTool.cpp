#include "stdafx.h"
#include "resource.h" 
#include <tlhelp32.h>

BOOL DropDecoyFileAndLaunch();
BOOL KillDecoyProcess();
BOOL CleanupDecoyFiles();

BOOL RunTool()
{
	// @@ First drop file into specified directory and launch
	if (!DropDecoyFileAndLaunch())
	{
		return false;
	}

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

	if (true == g_progopts.no_clean)
	{
		xprintf(OPLOG_NORMAL, L"# NOCLEAN is TRUE - NOT CLEANING UP AFTER TEST! \n");
		xprintf(OPLOG_NORMAL, L"# BE AWARE - ARTIFACTS WILL BE LEFT BEHIND! \n");
	}
	else
	{
		if (!KillDecoyProcess())
		{
			xprintf(OPLOG_NORMAL, L"! Decoy process not found or could not be killed. \n");
			return false;
		}

		Sleep(2000);

		// @@ Now remove fake malware file
		if (!CleanupDecoyFiles())
		{
			xprintf(OPLOG_NORMAL, L"! Failed to cleanup decoy files. \n");
			return false;
		}

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
	}

	return TRUE;
}

BOOL DropDecoyFileAndLaunch()
{
	WCHAR szFileName[MAX_PATH];
	WCHAR szFilePath[MAX_PATH];
	WCHAR szWorkingDirectory[MAX_PATH];

	if (TRUE == g_progopts.use_manual_filename)
	{
		_snwprintf_s(szFileName, MAX_PATH - 2, _TRUNCATE, L"%ws", g_progopts.file_name);
	}
	else
	{
		if (FALSE == GenerateFileName(g_progopts.filename_seed, szFileName))
		{
			xprintf(OPLOG_NORMAL, L"! Could not generate filename\n");
			return FALSE;
		}
	}

	switch (g_progopts.file_mode)
	{
	case PROG_FILEMODE::OPF_NONE:
		xprintf(OPLOG_NORMAL, L"! No file mode is configured or set by default\n");
		return false;
		break;
	case PROG_FILEMODE::OPF_CWD:
	{
		xprintf(OPLOG_NORMAL, L"[ Dropping decoy file into current working directory ]\n");

		if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_CWD, szFileName, szFilePath, szWorkingDirectory))
		{
			xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
			return FALSE;
		}

		ToolDropFileFromResource(szFilePath, MAKEINTRESOURCE(IDR_BINARY_DECOYCOPYDROP));
	}
	break;
	case PROG_FILEMODE::OPF_APPDATA:
	{
		xprintf(OPLOG_NORMAL, L"[ Dropping decoy file into AppData directory ]\n");

		if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_APPDATA, szFileName, szFilePath, szWorkingDirectory))
		{
			xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
			return FALSE;
		}

		ToolDropFileFromResource(szFilePath, MAKEINTRESOURCE(IDR_BINARY_DECOYCOPYDROP));
	}
	break;
	case PROG_FILEMODE::OPF_WINDOWS:
	{
		xprintf(OPLOG_NORMAL, L"[ Dropping decoy file into Windows directory ]\n");

		if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_APPDATA, szFileName, szFilePath, szWorkingDirectory))
		{
			xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
			return FALSE;
		}

		ToolDropFileFromResource(szFilePath, MAKEINTRESOURCE(IDR_BINARY_DECOYCOPYDROP));
	}
	break;
	}

	PROCESS_INFORMATION process_info;
	STARTUPINFO startup_info;

	memset(&process_info, 0, sizeof(process_info));
	memset(&startup_info, 0, sizeof(startup_info));

	startup_info.wShowWindow = SW_HIDE;

	xprintf(OPLOG_NORMAL, L"] Launching process from EXE at %ws\n", szFilePath);

	WCHAR szCommandLine[MAX_PATH];
	_snwprintf_s(szCommandLine, MAX_PATH - 2, _TRUNCATE, L"%ws %d DEPLOY", szFileName, g_progopts.filename_seed);

	CreateProcess(
		szFilePath,
		szCommandLine,
		NULL,
		NULL,
		TRUE,
		DETACHED_PROCESS,
		NULL,
		szWorkingDirectory,
		&startup_info,
		&process_info);

	CloseHandle(process_info.hThread);
	CloseHandle(process_info.hProcess);


	return TRUE;
}

BOOL KillDecoyProcess()
{
	WCHAR szFileName[MAX_PATH];

	if (TRUE == g_progopts.use_manual_filename)
	{
		_snwprintf_s(szFileName, MAX_PATH - 2, _TRUNCATE, L"%ws", g_progopts.file_name);
	}
	else
	{
		if (FALSE == GenerateFileName(g_progopts.filename_seed, szFileName))
		{
			xprintf(OPLOG_NORMAL, L"! Could not generate filename\n");
			return FALSE;
		}
	}

	HANDLE hProcessSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

	if (hProcessSnapShot == INVALID_HANDLE_VALUE)
	{
		xprintf(OPLOG_NORMAL, L"! CreateToolhelp32Snapshot error %08X \n", GetLastError() );
		return(FALSE);
	}	

	PROCESSENTRY32 ProcessEntry;
	ProcessEntry.dwSize = sizeof(ProcessEntry);

	if (!Process32First(hProcessSnapShot, &ProcessEntry))
	{
		xprintf(OPLOG_NORMAL, L"! Process32First error %08X \n", GetLastError());
		CloseHandle(hProcessSnapShot);          // clean the snapshot object
		return(FALSE);
	}

	do
	{
		if (0 == wcscmp(ProcessEntry.szExeFile, szFileName))
		{
			xprintf(OPLOG_NORMAL, L"] Terminating process %ws \n", ProcessEntry.szExeFile);

			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, ProcessEntry.th32ProcessID);
			TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
		}

	} while (Process32Next(hProcessSnapShot, &ProcessEntry));

	CloseHandle(hProcessSnapShot);

	return(TRUE);
}

BOOL CleanupDecoyFiles()
{
	WCHAR szFileName[MAX_PATH];
	WCHAR szFilePath[MAX_PATH];
	WCHAR szWorkingDirectory[MAX_PATH];

	if (TRUE == g_progopts.use_manual_filename)
	{
		_snwprintf_s(szFileName, MAX_PATH - 2, _TRUNCATE, L"%ws", g_progopts.file_name);
	}
	else
	{
		if (FALSE == GenerateFileName(g_progopts.filename_seed, szFileName))
		{
			xprintf(OPLOG_NORMAL, L"! Could not generate filename\n");
			return FALSE;
		}
	}

	switch (g_progopts.file_mode)
	{
	case PROG_FILEMODE::OPF_NONE:
		xprintf(OPLOG_NORMAL, L"! No file mode is configured or set by default\n");
		return false;
		break;
	case PROG_FILEMODE::OPF_CWD:
	{
		xprintf(OPLOG_NORMAL, L"[ Cleaning decoy file in current working directory ]\n");

		if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_CWD, szFileName, szFilePath, szWorkingDirectory))
		{
			xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
			return FALSE;
		}

		if (FALSE == DeleteFile(szFilePath))
		{
			xprintf(OPLOG_NORMAL, L"! Could not delete file, error %08X\n", GetLastError());
			return FALSE;
		}
	}
	break;
	case PROG_FILEMODE::OPF_APPDATA:
	{
		xprintf(OPLOG_NORMAL, L"[ Cleaning decoy file from AppData directory ]\n");

		if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_APPDATA, szFileName, szFilePath, szWorkingDirectory))
		{
			xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
			return FALSE;
		}

		if (FALSE == DeleteFile(szFilePath))
		{
			xprintf(OPLOG_NORMAL, L"! Could not delete file, error %08X\n", GetLastError());
			return FALSE;
		}
	}
	break;
	case PROG_FILEMODE::OPF_WINDOWS:
	{
		xprintf(OPLOG_NORMAL, L"[ Cleaning decoy file from Windows directory ]\n");

		if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_APPDATA, szFileName, szFilePath, szWorkingDirectory))
		{
			xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
			return FALSE;
		}

		if (FALSE == DeleteFile(szFilePath))
		{
			xprintf(OPLOG_NORMAL, L"! Could not delete file, error %08X\n", GetLastError());
			return FALSE;
		}
	}
	break;
	}

	// delete downstream artifacts
	// ---------------------------
	WCHAR szTmpPath[MAX_PATH];
	WCHAR szTmpFilename[MAX_PATH];
	WCHAR szTmpLogname[MAX_PATH];
	
	WCHAR szTmpFullPath[MAX_PATH];
	WCHAR szTmpLogFullPath[MAX_PATH];

	GetTempPath(MAX_PATH, szTmpPath);
	GenerateFileName(g_progopts.filename_seed, szTmpFilename);
	GenerateLogName(g_progopts.filename_seed, szTmpLogname);

	_snwprintf_s(szTmpFullPath, MAX_PATH - 2, _TRUNCATE, L"%ws\\%ws", szTmpPath, szTmpFilename);
	_snwprintf_s(szTmpLogFullPath, MAX_PATH - 2, _TRUNCATE, L"%ws\\%ws", szTmpPath, szTmpLogname);

	if (FALSE == DeleteFile(szTmpFullPath))
	{
		xprintf(OPLOG_NORMAL, L"! Could not delete temp EXE, error %08X\n", GetLastError());
		return FALSE;
	}

	if (FALSE == DeleteFile(szTmpLogFullPath))
	{
		xprintf(OPLOG_NORMAL, L"! Could not delete temp logfile, error %08X\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}