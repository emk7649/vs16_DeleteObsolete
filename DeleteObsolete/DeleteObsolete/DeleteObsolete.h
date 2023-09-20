#include <iostream>
#include <cstring>
#include <string>
#include <tchar.h>
//#include <minwindef.h>
//#include <processenv.h>
#include <windows.h>
//#include <tlhelp32.h>
#include <io.h>
#include <vector>
#include <direct.h>		//rmdir
#include <Shobjidl.h>
#include <shlwapi.h>
#include <atlbase.h>
#include <sys/stat.h>

#define THOUSAND 1000

#pragma warning(disable:4996)

inline bool exists_file (const std::string& name)
{
	struct stat buffer;   
	return (stat (name.c_str(), &buffer) == 0); 
}
inline void move_temp(const std::string& subPath, const std::string& dir_rename)
{
	// create folder
	for(int cnt=0; cnt < dir_rename.length(); cnt++)
	{
		if(dir_rename.at(cnt) == '\\')
		{
			std::string str_temp = dir_rename.substr(0, cnt);
			::CreateDirectory(str_temp.c_str(), NULL);
		}
	}
	rename(subPath.c_str(), dir_rename.c_str());
}

void FindTargetRecursive(std::string szDirectory, std::vector<char*> &listFolder, std::vector<char*> &listExtension, std::string dirtemp);
void DeleteOrBin(std::string szcPath, BOOL bDirectory);
HRESULT ThrowAwayPathToBin(std::string strPath);
int DeleteDirectory(const std::string &refcstrRootDirectory, bool bDeleteSubdirectories = true);
int strcmpLower(const char* str1, const char* str2);
long nMode; // 0:Del,  1:Bin,  2:Move
void main_abc()
{
	std::string dirname_new = "++Gratuitous++";
	while(1)
	{
		std::string s;
		getline(std::cin, s);
		if(s == "del")
		{
			nMode = 0;
			break;
		}
		if(s == "bin empty")
		{
			nMode = 1;
			//// Blow Trash Bin
			HRESULT hResult;
			hResult = SHEmptyRecycleBin(NULL, NULL, SHERB_NOPROGRESSUI || SHERB_NOCONFIRMATION);
			break;
		}
		if(s == "bin")
		{
			nMode = 1;
			break;
		}
		if(s == "move")
		{
			nMode = 2;
			break;
		}
		if(s == "take back")
		{
			nMode = 3;
			break;
		}
	}

	//1. 현재 경로(path) 알아내기 및 변경하기
	char strTemp_sz[THOUSAND];
	::GetModuleFileName(NULL, strTemp_sz, THOUSAND);
	std::string pathModule(strTemp_sz);
	
	//10. 상위 디렉토리(폴더)로 이동하기
	std::string pathFolder = pathModule.substr(0, pathModule.rfind('\\'));
	
	// TempFolder
	std::string pathTempFolder;
	int pos = pathFolder.rfind('\\');
	pathTempFolder = pathFolder.substr(0, pos + 1);
	pathTempFolder.insert(pos + 1, dirname_new);

	// when move, delete TempFolder and create it again.
	if(nMode == 2)
	{
		if(exists_file(pathTempFolder))
		{
			DeleteDirectory(pathTempFolder);
		}
		::CreateDirectory(pathTempFolder.c_str(), NULL);
	}

	std::vector<char*> deletingListFolder;
	deletingListFolder.push_back("ipch");
	deletingListFolder.push_back("debug");
	deletingListFolder.push_back("release");
	deletingListFolder.push_back("win32");
	deletingListFolder.push_back("x64");
	std::vector<char*> deletingListExtension;
	deletingListExtension.push_back(".sdf");
	deletingListExtension.push_back(".suo");

	FindTargetRecursive(pathFolder.c_str(), deletingListFolder, deletingListExtension, pathTempFolder);
	std::cout << "[done.]" << std::endl;

	if(nMode == 2)
	{
		while(1)
		{
			std::string s;
			getline(std::cin, s);
			if(s == "take back")
			{
				FindTargetRecursive(pathTempFolder.c_str(), deletingListFolder, deletingListExtension, pathFolder);
				DeleteDirectory(pathTempFolder);
				break;
			}
			if(s == "done")
			{
				break;
			}
		}
	}
	if(nMode == 3)
	{
		nMode = 2;
		FindTargetRecursive(pathTempFolder.c_str(), deletingListFolder, deletingListExtension, pathFolder);
		DeleteDirectory(pathTempFolder);
	}
}

void FindTargetRecursive(std::string szDirectory, std::vector<char*> &deletingListFolder, std::vector<char*> &deletingListExtension, std::string dirtemp)
{
	//7. 현재 폴더(디렉토리)의 파일 목록(리스트) 접근하기
	char path_c_file[THOUSAND];
	sprintf(path_c_file, "%s\\*", szDirectory);
	struct _finddata_t c_file;
	intptr_t hFile = _findfirst(path_c_file, &c_file);
	if(hFile==-1) return;      // no files
	do
	{
		if( !strcmp(c_file.name, ".") || !strcmp(c_file.name, ".."))
		{
			continue;
		}

		char subPath[THOUSAND];
		subPath[0] = '\0';
		char* backslash = "\\";
		strcat(subPath, szDirectory.c_str());
		strcat(subPath, backslash);
		strcat(subPath, c_file.name);
		
		//8. 경로 분해하기
		char fdrive[5];
		char fdir[THOUSAND];
		char fname[THOUSAND];
		char fext[10];
		_splitpath_s(subPath, fdrive, 5, fdir, 200, fname, 100, fext, 10);
		
		// Folder
		if(!strcmp(fext, ""))
		{
			std::string dir_rename = dirtemp;
			dir_rename.insert(dir_rename.size(), "\\");
			dir_rename.insert(dir_rename.size(), fname);
			for(long idx=0; idx < deletingListFolder.size(); idx++)
			{
				if(!strcmpLower(deletingListFolder[idx], c_file.name))
				{
					if(nMode == 2)
						move_temp(subPath, dir_rename);
						//rename(subPath, dir_rename.c_str());
					else
						DeleteOrBin(subPath, 1);
					goto NO_DELETE;
				}
			}
			FindTargetRecursive(subPath, deletingListFolder, deletingListExtension, dir_rename);
			NO_DELETE:;
		}
		// File
		else
		{
			for(long idx=0; idx < deletingListExtension.size(); idx++)
			{
				if(!strcmpLower(deletingListExtension[idx], fext))
				{
					std::string dir_rename = dirtemp;
					dir_rename.insert(dir_rename.size(), "\\");
					dir_rename.insert(dir_rename.size(), fname);
					dir_rename.insert(dir_rename.size(), fext);
					if(nMode == 2)
						move_temp(subPath, dir_rename);
						//rename(subPath, dir_rename.c_str());
					else
						DeleteOrBin(subPath, 0);
					break;
				}
			}
		}

	} while(_findnext(hFile, &c_file)==0);
	_findclose(hFile);
}

int strcmpLower(const char* szcStr1, const char* szcStr2)
{
	char Lower1[THOUSAND];
	strcpy(Lower1, szcStr1);
	for(int idx=0; idx < strlen(Lower1); idx++)
	{
		Lower1[idx] = tolower(Lower1[idx]);
	}

	char Lower2[THOUSAND];
	strcpy(Lower2, szcStr2);
	for(int idx=0; idx < strlen(Lower2); idx++)
	{
		Lower2[idx] = tolower(Lower2[idx]);
	}

	return strcmp(Lower1, Lower2);
}

void DeleteOrBin(std::string szcPath, BOOL bDirectory)
{
	if(nMode == 0)
	{
		if(bDirectory)
		{
			DeleteDirectory(szcPath);
		}
		else
		{
			std::remove(szcPath.c_str());
		}
		printf("del : %s\n", szcPath);
	}
	if(nMode == 1)
	{
		ThrowAwayPathToBin(szcPath);
		printf("bin : %s\n", szcPath);
	}
}

HRESULT ThrowAwayPathToBin(std::string szcPath)  
{  
    if (szcPath == "")  
        return E_POINTER;  
    // Unicode UTF-16 로 문자열 변경  
    CT2W wszFileToDelete( szcPath.c_str() );  
  
    // Initialize COM engine  
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);  
    if (SUCCEEDED(hr))  
    {  
        // Create COM instance of IFileOperation  
        IFileOperation *pfo = NULL;  
        hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pfo));  
  
        if (SUCCEEDED(hr))  
        {  
            // Set parameters for current operation  
            hr = pfo->SetOperationFlags(               
                FOF_SILENT |    // do not display progress dialog-box  
                FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR |  
                FOF_NOERRORUI   // do not display error message to the user  
                );  
            if (SUCCEEDED(hr))  
            {  
                // Create IShellItem instance associated to file to delete  
                IShellItem *psiFileToDelete = NULL;  
                hr = SHCreateItemFromParsingName(wszFileToDelete, NULL, IID_PPV_ARGS(&psiFileToDelete));  
                if (SUCCEEDED(hr))  
                {  
                    // Declare this shell item (file) to be deleted  
                    hr = pfo->DeleteItem( psiFileToDelete, NULL );  
                }  
                // Cleanup file-to-delete shell item  
                psiFileToDelete->Release();  
            }  
            if (SUCCEEDED(hr))  
            {  
                // Perform the deleting operation  
                hr = pfo->PerformOperations();  
            }  
        }  
        // Cleanup file operation object  
        pfo->Release();  
    }  
    // Cleanup COM  
    CoUninitialize();  
    return hr;  
}  

int DeleteDirectory(const std::string &refcstrRootDirectory, bool bDeleteSubdirectories)
{
	bool            bSubdirectory = false;       // Flag, indicating whether
	// subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	std::string     strFilePath;                 // Filepath
	std::string     strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information

	strPattern = refcstrRootDirectory + _T("\\*.*");
	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (_tcscmp(FileInformation.cFileName, _T(".")) && _tcscmp(FileInformation.cFileName, _T("..")))
			{
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + _T("\\") + FileInformation.cFileName;

				if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if(bDeleteSubdirectories)
					{
						// Delete subdirectory
						int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
						if(iRC)
							return iRC;
					}
					else
						bSubdirectory = true;
				}
				else
				{
					// Set file attributes
					if(::SetFileAttributes(strFilePath.c_str(),
						FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();

					// Delete file
					if(::DeleteFile(strFilePath.c_str()) == FALSE)
						return ::GetLastError();
				}
			}
		} while(::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if(dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else
		{
			if(!bSubdirectory)
			{
				// Set directory attributes
				if(::SetFileAttributes(refcstrRootDirectory.c_str(),
					FILE_ATTRIBUTE_NORMAL) == FALSE)
					return ::GetLastError();

				// Delete directory
				if(::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
					return ::GetLastError();
			}
		}
	}

	return 0;
}