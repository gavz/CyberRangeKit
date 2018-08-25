#include "stdafx.h"
#include "dropservice.h"

BOOL EnableAdminPermissions();

BOOL ToolCreatePersistentService(WCHAR *theBinaryPath, WCHAR *theServiceName, WCHAR *theServiceDisplayName)
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;

	//EnableAdminPermissions();

	/*
	HANDLE hUserToken;
	if (FALSE == LogonUser(L"NETWORK SERVICE", L"NT_AUTHORITY", NULL, LOGON32_LOGON_NEW_CREDENTIALS, LOGON32_PROVIDER_WINNT50, &hUserToken))
	{
		xprintf(OPLOG_NORMAL, L"! Could not elevate privilege, error %08X\n", GetLastError());
		return FALSE;
	}

	if (FALSE == ImpersonateLoggedOnUser(hUserToken))
	{
		xprintf(OPLOG_NORMAL, L"! Could not impersonate user, error %08X\n", GetLastError());
		return FALSE;
	}
	*/

	hSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == hSCManager)
	{
		xprintf(OPLOG_NORMAL, L"! Could not open Service Control Manager, error %08X\n", GetLastError());
		return FALSE;
	}

	hService = CreateService(
		hSCManager,
		theServiceName,
		theServiceDisplayName,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,			// don't auto-start, it's not required for this tool
		SERVICE_ERROR_NORMAL,
		theBinaryPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	if (hService == NULL)
	{
		DWORD dwError = GetLastError();

		xprintf(OPLOG_NORMAL, L"! Create Service error %08X\n", dwError);

		switch (dwError)
		{
			case ERROR_SERVICE_MARKED_FOR_DELETE:
			{
				// typical error when a service doesn't shut down properly and you try to re-install
				// you probably need to reboot computer now to clear this condition
				xprintf(OPLOG_NORMAL, L"! Service marked for deletion\n");
			}
			break;

			case ERROR_SERVICE_EXISTS:
			{
				// tried to create it twice
				xprintf(OPLOG_NORMAL, L"! Service already exists\n");
			}
			break;
		}

		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	// don't attempt to start the service - it's not required for this tool
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	
	return TRUE;
}

BOOL ToolDeletePersistentService(WCHAR *theServiceName)
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;

	hSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == hSCManager)
	{
		xprintf(OPLOG_NORMAL, L"! Could not open Service Control Manager, error %08X\n", GetLastError());
		return FALSE;
	}

	hService = OpenService(
		hSCManager,
		theServiceName,
		SERVICE_ALL_ACCESS
		);

	if (hService == NULL)
	{
		DWORD dwError = GetLastError();

		xprintf(OPLOG_NORMAL, L"! Open Service error %08X\n", dwError);

		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	if (FALSE == DeleteService(hService))
	{
		DWORD dwError = GetLastError();

		xprintf(OPLOG_NORMAL, L"! Delete Service error %08X\n", dwError);

		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);

		return FALSE;
	}

	// don't attempt to start the service - it's not required for this tool
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return TRUE;
}

BOOL EnableAdminPermissions()
{
	LUID luid;
	TOKEN_PRIVILEGES privs;
	HANDLE hToken = NULL;
	DWORD dwBufLen = 0;
	char buf[1024];
	HANDLE hProcess = GetCurrentProcess();

	ZeroMemory(&luid, sizeof(luid));

	if (!LookupPrivilegeValue(NULL, SE_TCB_NAME, &luid))
		return FALSE;

	privs.PrivilegeCount = 1;
	privs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	memcpy(&privs.Privileges[0].Luid, &luid, sizeof(privs.Privileges[0].Luid));

	if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
		return FALSE;

	if (!AdjustTokenPrivileges(hToken, FALSE, &privs, sizeof(buf), (PTOKEN_PRIVILEGES)buf, &dwBufLen))
		return FALSE;

	return TRUE;
}