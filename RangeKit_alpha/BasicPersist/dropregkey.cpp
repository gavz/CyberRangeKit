#include "stdafx.h"
#include "dropregkey.h"

BOOL ToolAddRegKeyForExe(PROG_PERSISTMODE theMode, WCHAR *theValueName, WCHAR *theFilePath)
{
	LSTATUS status;

	HKEY hKey = NULL;
	HKEY hKeyRoot = HKEY_CURRENT_USER; // of HKEY_CLASSES_ROOT HKEY_CURRENT_CONFIG HKEY_CURRENT_USER HKEY_LOCAL_MACHINE HKEY_USERS
	
	status = RegOpenKeyEx(
				hKeyRoot, 
				L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",   // Note key names are not case sensitive.
				0,
				KEY_SET_VALUE,
				&hKey);

	if (status != ERROR_SUCCESS)
	{
		xprintf(OPLOG_NORMAL, L"! Could not open Run key. Error:%08X\n", GetLastError());
		return FALSE;
	}

	WCHAR *some_data = theFilePath;
	WCHAR *some_value = theValueName;

	status = RegSetValueEx(
				hKey,
				some_value, 
				0,										// reserved
				REG_SZ,									// type of value
				(const BYTE *)some_data,
				((DWORD)wcslen(some_data) + 1) * 2);	// length IN BYTES including NULL CHARACTER!
	
	if (status != ERROR_SUCCESS)
	{
		xprintf(OPLOG_NORMAL, L"! Could not set registry value. Error:%08X\n", GetLastError());
		
		if (CloseHandle(hKey) == 0)
		{
			xprintf(OPLOG_NORMAL, L"! Error attempting to close registry key handle. ERROR: %08X\n", GetLastError());
			return FALSE;
		}

		return FALSE;
	}

	if (CloseHandle(hKey) == 0)
	{
		xprintf(OPLOG_NORMAL, L"! Error attempting to close registry key handle. ERROR: %08X\n", GetLastError());
		return FALSE;
	}
	
	return TRUE;
}

BOOL ToolRemoveRegKeyForExe(PROG_PERSISTMODE theMode, WCHAR *theValueName)
{
	LSTATUS status;

	HKEY hKey = NULL;
	HKEY hKeyRoot = HKEY_CURRENT_USER; // of HKEY_CLASSES_ROOT HKEY_CURRENT_CONFIG HKEY_CURRENT_USER HKEY_LOCAL_MACHINE HKEY_USERS

	status = RegOpenKeyEx(
		hKeyRoot,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",   // Note key names are not case sensitive.
		0,
		KEY_SET_VALUE,
		&hKey);

	if (status != ERROR_SUCCESS)
	{
		xprintf(OPLOG_NORMAL, L"! Could not open Run key. Error:%08X\n", GetLastError());
		return FALSE;
	}

	WCHAR *some_value = theValueName;

	status = RegDeleteValue(
		hKey,
		some_value);

	if (status != ERROR_SUCCESS)
	{
		xprintf(OPLOG_NORMAL, L"! Could not delete registry value. Error:%08X\n", GetLastError());

		if (CloseHandle(hKey) == 0)
		{
			xprintf(OPLOG_NORMAL, L"! Error attempting to close registry key handle. ERROR: %08X\n", GetLastError());
			return FALSE;
		}

		return FALSE;
	}

	if (CloseHandle(hKey) == 0)
	{
		xprintf(OPLOG_NORMAL, L"! Error attempting to close registry key handle. ERROR: %08X\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}