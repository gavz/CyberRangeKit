
#include "stdafx.h"
#include "wgetopt.h"

WCHAR	*optarg;		// points to argument passed with option (if any, otherwise NULL)
WCHAR	*optmod;		// points to modifer passed with option (aka '-p10' the modifier is '10', '-pAX' the modifier is 'AX')
int		optind = 0; 	// global argv index

// this is called once for each arg, which is why optind is global
// returns the option flag, optarg holds the optional arg if the flag is decorated with ':'
WCHAR wgetopt(int argc, WCHAR *argv[], WCHAR *optstring)
{
	static WCHAR *next = NULL;

	// sentinel to initialize
	if (optind == 0)
		next = NULL;

	optarg = NULL;
	optmod = NULL;

	if (next == NULL || *next == L'\0')
	{
		// first trip thru (argv[0] is the command line so we want to skip to argv[1] for first arg)
		if (optind == 0)
			optind++;

		// end of args
		if (optind >= argc)
		{
			return NULL;
		}

		// not prepended with dash, or its a trailing dash with no option, treat as syntax error
		if (argv[optind][0] != L'-' || argv[optind][1] == L'\0')
		{
			return L'?';
		}

		next = argv[optind];
		next++;		// skip past -
		optind++;
	}

	WCHAR c = *next++; // get first character of option
	WCHAR *cp = wcschr(optstring, c); // point to position of option character in optstring

	// if there is no option character in optstring, 
	// OR the option passed was ':' (aka -: or -:something)
	if (cp == NULL || c == L':')
		return L'?';

	// if its NOT the end of the argument (aka '-pAX')
	if (*next != L'\0')
	{
		optmod = next; // optmod becomes 'AX'
		next = NULL;
	}

	cp++; // check the next char in optstring for modifier

	// expects a optional argument to be passed (aka 'p:')
	if (*cp == L':' || c == L'-')
	{
		if (optind < argc)
		{
			optarg = argv[optind];

			if (c == L'-' && *optarg != L'-')
			{
				// in the case of --something, the next argument is NOT a flag
				// so continue to treat it as the optarg
				optind++;
			}
			else if (c == L'-' && *optarg == L'-')
			{
				// in the case of --something, the next argument is a flag
				// treat this as if no optarg was passed (not an error)
				optarg = NULL;
			}
			else if ( c != L'-' && *optarg == L'-')
			{
				// in the case of a normal -p flag (not --something)
				// the optarg looks like another option. Treat this as a potential error
				// aka -p1 -d1 -c2 <-- don't treat '-d1' as the optarg for -p1
				// instead assume they forgot to put the optarg on -p1
				return L'?';
			}
			else
			{
				// in this case the optarg is not a flag so let's use it
				optind++;
			}
		}
		else if (c == L'-')
		{
			// it was a --something argument with no additional arguments, this is fine
			optarg = NULL;
		}
		else
		{
			return L'?';
		}
	}

	return c;
}