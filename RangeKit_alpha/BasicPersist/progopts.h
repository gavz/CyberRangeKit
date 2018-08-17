#pragma once

enum PROG_LOGMODE
{
	OPLOG_QUIET = 2,
	OPLOG_NORMAL = 4,
	OPLOG_VERBOSE = 8,
	OPLOG_DEBUG = 16,

	OPLOG_LAST
};

enum PROG_FILEMODE
{
	OPF_NONE,
	OPF_CWD,
	OPF_APPDATA,
	OPF_WINDOWS,

	OPF_LAST
};

enum PROG_PERSISTMODE
{
	OPP_NONE,
	OPP_EXISTINGSERVICE,
	OPP_NEWSERVICE,
	OPP_NEWTASK,
	OPP_RUNKEY,

	OPP_LAST
};

struct prog_opts
{
	PROG_LOGMODE		log_mode = OPLOG_NORMAL;
	PROG_FILEMODE		file_mode = OPF_CWD;
	PROG_PERSISTMODE	persist_mode = OPP_RUNKEY;
	bool				pretty_print = false;
	bool				use_wait_time = false;
	DWORD				wait_time = 0;
	bool				no_clean = false;

	int					filename_seed = 0;
	int					kst_seed = 0;					// kst == key service or task (by context)

	bool				use_manual_filename = false;
	WCHAR				file_name[MAX_PATH];

	bool				use_manual_kstname = false;		// kst == key service or task (by context)
	WCHAR				kst_name[MAX_PATH];
};

extern prog_opts g_progopts;

