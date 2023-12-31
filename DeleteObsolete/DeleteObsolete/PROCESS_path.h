#pragma once
#include <windows.h>

int strcmpLower(const char* szcStr1, const char* szcStr2)
{
	char Lower1[MAX_PATH];
	strcpy_s(Lower1, szcStr1);
	for (int idx = 0; idx < strlen(Lower1); idx++)
	{
		Lower1[idx] = tolower(Lower1[idx]);
	}

	char Lower2[MAX_PATH];
	strcpy_s(Lower2, szcStr2);
	for (int idx = 0; idx < strlen(Lower2); idx++)
	{
		Lower2[idx] = tolower(Lower2[idx]);
	}

	return strcmp(Lower1, Lower2);
}

void MovePathMakingFolder(const std::string& subPath, const std::string& dir_rename)
{
	// create folder
	for (int cnt = 0; cnt < dir_rename.length(); cnt++)
	{
		if (dir_rename.at(cnt) == '\\')
		{
			std::string str_temp = dir_rename.substr(0, cnt);
			::CreateDirectoryA(str_temp.c_str(), NULL);
		}
	}
	rename(subPath.c_str(), dir_rename.c_str());
}

bool DoDirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

int DeleteDirectory(const tstring& refcstrRootDirectory, bool bDeleteSubdirectories = true)
{
	bool            bSubdirectory = false;       // Flag, indicating whether
	// subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	tstring     strFilePath;                 // Filepath
	tstring     strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information

	strPattern = refcstrRootDirectory + _T("\\*.*");
	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (_tcscmp(FileInformation.cFileName, _T(".")) && _tcscmp(FileInformation.cFileName, _T("..")))
			{
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + _T("\\") + FileInformation.cFileName;

				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bDeleteSubdirectories)
					{
						// Delete subdirectory
						int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
						if (iRC)
							return iRC;
					}
					else
						bSubdirectory = true;
				}
				else
				{
					// Set file attributes
					if (::SetFileAttributes(strFilePath.c_str(),
						FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();

					// Delete file
					if (::DeleteFile(strFilePath.c_str()) == FALSE)
						return ::GetLastError();
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else
		{
			if (!bSubdirectory)
			{
				// Set directory attributes
				if (::SetFileAttributes(refcstrRootDirectory.c_str(),
					FILE_ATTRIBUTE_NORMAL) == FALSE)
					return ::GetLastError();

				// Delete directory
				if (::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
					return ::GetLastError();
			}
		}
	}

	return 0;
}