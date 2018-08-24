#include "stdafx.h"
#include "generate.h"
#include "dropfile.h"
#include "dropregkey.h"

BOOL CreateAppendLogFile();
void FakeRunTool();

BOOL DropDecoyFile();
BOOL CleanupDecoyFile();
BOOL CreatePersistence();
BOOL CleanupPersistence();

BOOL RunTool()
{
	// uncomment for testing...
	//FakeRunTool();
	
	// @@ First drop file
	//    Attempt to drop fake malware decoy into specified directory
	if (!DropDecoyFile())
	{
		return false;
	}

	Sleep(1000);
	
	// @@ Now create persistence
	//    Create artifact to survive reboot
	if (!CreatePersistence())
	{
		CleanupDecoyFile();
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
		// @@ Now remove persistence artifact
		if (!CleanupPersistence())
		{
			CleanupDecoyFile();
			return false;
		}

		Sleep(1000);

		// @@ Now remove fake malware file
		if (!CleanupDecoyFile())
		{
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

	return true;
}

void FakeRunTool()
{
	xprintf(OPLOG_NORMAL, L"[ TOOL RUNNING.");
	for (int i = 0; i < 5; i++)
	{
		xputchar(OPLOG_NORMAL, L'[', L'.');
		Sleep(1000);
	}

	// closing bracket
	xputchar(OPLOG_NORMAL, L'[', L' ');
	xputchar(OPLOG_NORMAL, L'[', L']');

	xprintf(OPLOG_NORMAL, L"\n");
	xprintf(OPLOG_NORMAL, L"[ TOOL DONE. ]\n");

	return;
}

BOOL DropDecoyFile()
{
	WCHAR szFileName[MAX_PATH];
	WCHAR szFilePath[MAX_PATH];

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

			if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_CWD, szFileName, szFilePath))
			{
				xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
				return FALSE;
			}

			// TESTING
			//ToolDropFileEmpty(szFilePath);
			ToolDropFileFromResource(szFilePath);
		}
		break;
	case PROG_FILEMODE::OPF_APPDATA:
		{
			xprintf(OPLOG_NORMAL, L"[ Dropping decoy file into AppData directory ]\n");

			if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_APPDATA, szFileName, szFilePath))
			{
				xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
				return FALSE;
			}

			ToolDropFileEmpty(szFilePath);
		}
		break;
	case PROG_FILEMODE::OPF_WINDOWS:
		{
			xprintf(OPLOG_NORMAL, L"[ Dropping decoy file into Windows directory ]\n");

			if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_APPDATA, szFileName, szFilePath))
			{
				xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
				return FALSE;
			}

			ToolDropFileEmpty(szFilePath);
		}
		break;
	}

	return TRUE;
}

BOOL CleanupDecoyFile()
{
	WCHAR szFileName[MAX_PATH];
	WCHAR szFilePath[MAX_PATH];

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
			if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_CWD, szFileName, szFilePath))
			{
				xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
				return FALSE;
			}

			xprintf(OPLOG_NORMAL, L"[ Cleaning up decoy file in current working directory ]\n");
			ToolCleanFile(szFilePath);
		}
		break;
	case PROG_FILEMODE::OPF_APPDATA:
		{
			if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_APPDATA, szFileName, szFilePath))
			{
				xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
				return FALSE;
			}

			xprintf(OPLOG_NORMAL, L"[ Cleaning up decoy file in AppData directory ]\n");
			ToolCleanFile(szFilePath);
		}
		break;
	case PROG_FILEMODE::OPF_WINDOWS:
		{
			if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_WINDOWS, szFileName, szFilePath))
			{
				xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
				return FALSE;
			}

			xprintf(OPLOG_NORMAL, L"[ Cleaning up decoy file in Windows directory ]\n");
			ToolCleanFile(szFilePath);
		}
		break;
	}

	return TRUE;
}

BOOL CreatePersistence()
{
	switch (g_progopts.persist_mode)
	{
	case PROG_PERSISTMODE::OPP_NONE:
		xprintf(OPLOG_NORMAL, L"! No persist mode is configured or set by default\n");
		return false;
	case PROG_PERSISTMODE::OPP_EXISTINGSERVICE:
		xprintf(OPLOG_NORMAL, L"[ Using existing service for persistence ]\n");
		break;
	case PROG_PERSISTMODE::OPP_NEWSERVICE:
		xprintf(OPLOG_NORMAL, L"[ Creating new service for persistence ]\n");
		break;
	case PROG_PERSISTMODE::OPP_NEWTASK:
		xprintf(OPLOG_NORMAL, L"[ Creating new task for persistence ]\n");
		break;
	case PROG_PERSISTMODE::OPP_RUNKEY:
		{
			xprintf(OPLOG_NORMAL, L"[ Using Run key for persistence ]\n");
			
			WCHAR szFileName[MAX_PATH];
			WCHAR szFilePath[MAX_PATH];
			WCHAR szValueName[MAX_PATH];

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

			if (FALSE == GenerateFilePath(PROG_FILEMODE::OPF_CWD, szFileName, szFilePath))
			{
				xprintf(OPLOG_NORMAL, L"! Could not generate filepath\n");
				return FALSE;
			}

			if (TRUE == g_progopts.use_manual_kstname)
			{
				_snwprintf_s(szFileName, MAX_PATH - 2, _TRUNCATE, L"%ws", g_progopts.kst_name);
			}
			else
			{
				if (FALSE == GenerateRegValueNameForExe(g_progopts.kst_seed, szValueName))
				{
					xprintf(OPLOG_NORMAL, L"! Could not generate reg value name\n");
					return FALSE;
				}
			}

			ToolAddRegKeyForExe(PROG_PERSISTMODE::OPP_RUNKEY, szValueName, szFilePath);
		}
		break;
	}
	return TRUE;
}

BOOL CleanupPersistence()
{
	switch (g_progopts.persist_mode)
	{
	case PROG_PERSISTMODE::OPP_NONE:
		xprintf(OPLOG_NORMAL, L"! No persist mode is configured or set by default\n");
		return FALSE;
	case PROG_PERSISTMODE::OPP_EXISTINGSERVICE:
		xprintf(OPLOG_NORMAL, L"[ Restoring existing service used for persistence ]\n");
		break;
	case PROG_PERSISTMODE::OPP_NEWSERVICE:
		xprintf(OPLOG_NORMAL, L"[ Removing new service used for persistence ]\n");
		break;
	case PROG_PERSISTMODE::OPP_NEWTASK:
		xprintf(OPLOG_NORMAL, L"[ Removing new task used for persistence ]\n");
		break;
	case PROG_PERSISTMODE::OPP_RUNKEY:
		{
			xprintf(OPLOG_NORMAL, L"[ Removing Run key used for persistence ]\n");

			WCHAR szValueName[MAX_PATH];

			if (FALSE == GenerateRegValueNameForExe(0, szValueName))
			{
				xprintf(OPLOG_NORMAL, L"! Could not generate reg value name\n");
				return FALSE;
			}

			ToolRemoveRegKeyForExe(PROG_PERSISTMODE::OPP_RUNKEY, szValueName);
		}
		break;
	}
	return TRUE;
}

BOOL CreateAppendLogFile()
{
	PROCESS_INFORMATION process_info;
	STARTUPINFO startup_info;

	WCHAR szWorkingDirectory[MAX_PATH];
	WCHAR *envName = L"CD";

	switch (g_progopts.file_mode)
	{
	case PROG_FILEMODE::OPF_APPDATA:
		envName = L"APPDATA";
		break;
	case PROG_FILEMODE::OPF_WINDOWS:
		envName = L"WINDIR";
		break;
	}

	if (GetEnvironmentVariable(envName, szWorkingDirectory, MAX_PATH) == 0)
	{
		xprintf(OPLOG_NORMAL, L"Could not get environment variable\n");
		return FALSE;
	}

	memset(&process_info, 0, sizeof(process_info));
	memset(&startup_info, 0, sizeof(startup_info));

	startup_info.wShowWindow = SW_HIDE;

	xprintf(OPLOG_NORMAL, L"Appending log file to %ws\\coms.log\n", szWorkingDirectory);

	CreateProcess(
		L"c:\\windows\\system32\\cmd.exe",
		L"cmd.exe /c echo hello world >> coms.log",
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