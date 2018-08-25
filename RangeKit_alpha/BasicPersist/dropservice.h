#pragma once

BOOL ToolCreatePersistentService(WCHAR *theBinaryPath, WCHAR *theServiceName, WCHAR *theServiceDisplayName);
BOOL ToolDeletePersistentService(WCHAR *theServiceName);