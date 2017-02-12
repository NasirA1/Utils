#pragma once
#include <string>
#include <afx.h>
#include <io.h>


enum class AccessModes
{
	ExistenceOnly = 0,
	WriteOnly = 2,
	ReadOnly = 4,
	ReadWrite = 6
};


template<typename TChar = char>
static inline bool CanAccess(const std::basic_string<TChar>& path, const AccessModes mode)
{
	return _taccess(path.c_str(), static_cast<int>(mode)) != -1;
}



template<typename Func>
static inline void WalkDirectory(const LPCTSTR pstr, Func& action)
{
	CFileFind finder;

	// build a string with wildcards
	CString strWildcard(pstr);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		if (finder.IsDots())
			continue;

		auto str = finder.GetFilePath();

		// if it's a directory, recursively search it
		if (finder.IsDirectory())
		{
			WalkDirectory(str, action);
		}
		else
		{
			action(str);
		}
	}

	finder.Close();
}
