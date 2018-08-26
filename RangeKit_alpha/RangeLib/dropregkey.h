#pragma once

BOOL ToolAddRegKeyForExe(PROG_PERSISTMODE theMode, WCHAR *theValueName, WCHAR *theFilePath);
BOOL ToolRemoveRegKeyForExe(PROG_PERSISTMODE theMode, WCHAR *theValueName);