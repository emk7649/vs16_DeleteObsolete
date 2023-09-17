#include "tstring.h"
#include "header1.h"

#include <cstdio>
#include <tchar.h>
#include <windows.h>

#include <conio.h>
#include <locale.h>

#include <string>
#include <iostream>
#include <vector>

#ifdef _DEBUG
//#define new DEBUG_NEW
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>


long g_nMode; // 0:del(default),  1:bin,  2:move, 3:bringback
int GetMode();
void FindTargetRecursive(std::vector<tstring>& pathsHit,  tstring szDirectory, std::vector<tstring>& deletingListFolder, std::vector<tstring>& deletingListExtension);

int main_func(int argc, TCHAR* argv[]);
int _tmain(int argc, TCHAR* argv[])
{
    //_CrtSetBreakAlloc(86);
    _tsetlocale(LC_ALL, _T("korean"));
    int exit_code = main_func(argc, argv);
    _CrtDumpMemoryLeaks();
    return exit_code;
}
int main_func(int argc, TCHAR* argv[])
{
    //DeleteDirectory(s);

    //getline(s);
    //_tprintf(s.c_str());
    //std::wstring abc = WSTRING(s);
    //int size_s = abc.length();

    //////////////////////////////////////////////////////////////////////////////////////////
    // ready
    
    // 1. 현재 경로(path)
    TCHAR strTemp_sz[MAX_PATH];
    ::GetModuleFileName(NULL, strTemp_sz, MAX_PATH);
    tstring pathModuleFileName(strTemp_sz);
    tstring pathFolder = pathModuleFileName.substr(0, pathModuleFileName.rfind('\\'));


    // emk debug 
    pathFolder = _T("C:\\code\\git\\vs16_DeleteObsolete\\DeleteObsolete\\DeleteObsolete\\Release");


    // 2. Obsolete folder
    tstring pathObsolete;
    int pos = pathFolder.rfind('\\');
    if (pos == -1)
    {
        pathObsolete = _T("");
    }
    else
    {
        pathObsolete = pathFolder.substr(0, pos + 1);
        pathObsolete.insert(pos + 1, _T("++Obsolete++"));
    }

    // 3. list keyword
    std::vector<tstring> keywordsFolder;
    keywordsFolder.push_back(_T("ipch"));
    keywordsFolder.push_back(_T("debug"));
    keywordsFolder.push_back(_T("release"));
    keywordsFolder.push_back(_T("win32"));
    keywordsFolder.push_back(_T("x64"));
    keywordsFolder.push_back(_T(".vs"));  // vs16
    std::vector<tstring> keywordsExtension;
    keywordsExtension.push_back(_T(".sdf"));
    keywordsExtension.push_back(_T(".suo"));

    // 4. list Folders and Files to Delete
    std::vector<tstring> pathsHit;
    FindTargetRecursive(pathsHit, pathFolder, keywordsFolder, keywordsExtension);

    // end ready
    //----------------------------------------------------------------------------------------


    //////////////////////////////////////////////////////////////////////////////////////////
    // loop
    //_tprintf(_T("select one (del, move, bring back, bin, bin empty"));
    while (1)
    {
        int nMode = GetMode();
        switch (nMode)
        {
        case 0: // del
        {
        }
        return 0;
        case 1: // move
        {
            if(pathObsolete == _T(""))
                _tprintf(_T(" This is not right directory to move\n"));

        }
        break;
        case 2: // bring back
        {
            if (GetFileAttributes(pathObsolete.c_str()) == INVALID_FILE_ATTRIBUTES)
                _tprintf(_T(" There is no obsolete directory to bring back\n"));

        }
        break;
        case 3: // bin
        {
        }
        return 0;
        case 4: // bin empty
        {
            //// Blow Trash Bin
            HRESULT hResult;
            hResult = SHEmptyRecycleBin(NULL, NULL, SHERB_NOPROGRESSUI || SHERB_NOCONFIRMATION);
            break;
        }
        case 5: // quit
            return 0;
        default:;
        }
    }
    
    // end loop
    //----------------------------------------------------------------------------------------
    return 0;
}






int GetMode()
{
    _tprintf(_T("\nselect one (del, move, bring back, bin, bin empty, quit\n"));

    std::string s;
    getline(std::cin, s);
    if (s == "del")
    {
        return 0;
    }
    if (s == "move")
    {
        return 1;
    }
    if (s == "take back")
    {
        return 2;
    }
    if (s == "bin")
    {
        return 3;
    }
    if (s == "bin empty")
    {
        return 4;
    }
    if (s == "quit")
    {
        return 5;
    }
    if (s == "q")
    {
        return 5;
    }
}

#include <io.h>
void FindTargetRecursive(std::vector<tstring>& pathsHit, tstring szDirectory, std::vector<tstring>& deletingListFolder, std::vector<tstring>& deletingListExtension)
{
    //7. 현재 폴더(디렉토리)의 파일 목록(리스트) 접근하기
    std::string path_c_file = STRING(szDirectory) + "\\*";
    struct _finddata_t c_file;
    intptr_t hFile = _findfirst(path_c_file.c_str(), &c_file);
    if (hFile == -1) // no files
        return;

    do
    {
        if (!strcmp(c_file.name, ".") || !strcmp(c_file.name, ".."))
        {
            continue;
        }

        char subPath[MAX_PATH];
        subPath[0] = '\0';
        const char* backslash = "\\";
        strcat_s(subPath, STRING(szDirectory).c_str());
        strcat_s(subPath, backslash);
        strcat_s(subPath, c_file.name);

        //8. 경로 분해하기
        char fdrive[5];
        char fdir[MAX_PATH];
        char fname[MAX_PATH];
        char fext[10];
        _splitpath_s(subPath, fdrive, 5, fdir, 200, fname, 100, fext, 10);
        std::string subPathS = subPath;

        // Folder
        if (!strcmp(fext, ""))
        {
            /*
            std::string dir_rename = STRING(dirtemp);
            dir_rename.insert(dir_rename.size(), "\\");
            dir_rename.insert(dir_rename.size(), fname);
            */
            bool bHit = false;
            for (long idx = 0; idx < deletingListFolder.size(); idx++)
            {
                if (!strcmpLower(STRING(deletingListFolder[idx]).c_str(), c_file.name))
                {
                    pathsHit.push_back(TSTRING(subPathS));
                    bHit = true;
                    break;
                }
            }
            if(!bHit)
                FindTargetRecursive(pathsHit, TSTRING(subPathS), deletingListFolder, deletingListExtension);
        }
        // File
        else
        {
            for (long idx = 0; idx < deletingListExtension.size(); idx++)
            {
                if (!strcmpLower(STRING(deletingListExtension[idx]).c_str(), fext))
                {
                    pathsHit.push_back(TSTRING(subPathS));
                }
            }
        }

    } while (_findnext(hFile, &c_file) == 0);
    _findclose(hFile);
    
}