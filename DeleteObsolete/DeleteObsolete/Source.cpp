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
    tstring s = _T("C:\\code\\git\\vs16_DeleteObsolete\\DeleteObsolete\\DeleteObsolete\\Release");
    _tprintf(_T("한글\n"));
    DeleteDirectory(s);
    //getline(s);

    //_tprintf(s.c_str());
    //std::wstring abc = WSTRING(s);
    //int size_s = abc.length();

    //////////////////////////////////////////////////////////////////////////////////////////
    // ready
    
    //1. 현재 경로(path)
    TCHAR strTemp_sz[MAX_PATH];
    ::GetModuleFileName(NULL, strTemp_sz, MAX_PATH);
    tstring pathModuleFileName(strTemp_sz);
    tstring pathFolder = pathModuleFileName.substr(0, pathModuleFileName.rfind('\\'));

    //2. Temp folder
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

    //3. list
    std::vector<tstring> listFolder_toDelete;
    listFolder_toDelete.push_back(_T("ipch"));
    listFolder_toDelete.push_back(_T("debug"));
    listFolder_toDelete.push_back(_T("release"));
    listFolder_toDelete.push_back(_T("win32"));
    listFolder_toDelete.push_back(_T("x64"));
    listFolder_toDelete.push_back(_T(".vs"));  // vs16
    std::vector<tstring> listExtention_toDelete;
    listExtention_toDelete.push_back(_T(".sdf"));
    listExtention_toDelete.push_back(_T(".suo"));

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
                _tprintf(_T(" This is not obsolete directory to bring back\n"));

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