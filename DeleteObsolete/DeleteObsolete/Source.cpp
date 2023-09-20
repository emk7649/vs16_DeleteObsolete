#include <cstdio>
#include <tchar.h>
#include <windows.h>

#include <conio.h>
#include <locale.h>

#include <string>
#include <iostream>
#include <vector>

#include "tstring.h"
#include "PROCESS_path.h"
#include "header1.h"
#include "ThreadEngine.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

extern CRITICAL_SECTION g_csLog;

ENUM_MODE GetMode();
void FindTargetRecursive(std::vector<tstring>& pathsHit,  tstring szDirectory, std::vector<tstring>& deletingListFolder, std::vector<tstring>& deletingListExtension);

int main_func(int argc, TCHAR* argv[]);
int _tmain(int argc, TCHAR* argv[])
{
    //_CrtSetBreakAlloc(86);

    _tsetlocale(LC_ALL, _T("korean"));
    InitializeCriticalSection(&g_csLog);
    int exit_code = main_func(argc, argv);
    DeleteCriticalSection(&g_csLog);


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
    // (Not Match Case), 폴더는 정확히 일치, 파일은 확장자 일치
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

    std::vector<tstring> logs;
    CThreadEngine_ProcessingPaths threadPath;
    threadPath.m_pathsHit = pathsHit;
    threadPath.m_pLogs = &logs;

    CThreadEngine_Print threadPrint;
    threadPrint.m_pLogs = &logs;
    threadPrint.Run();
    // end ready
    //----------------------------------------------------------------------------------------


    //////////////////////////////////////////////////////////////////////////////////////////
    // loop
    //_tprintf(_T("select one (del, move, bring back, bin, bin empty"));
    while (1)
    {
        ENUM_MODE nMode = GetMode();
        switch (nMode)
        {
        case ENUM_MODE::DEL:
        {
            threadPath.m_nMode = nMode;
            threadPath.Run();
            if (!threadPath.WaitRunEnd())
            {
            }
            _tprintf(_T("	%.1f [ms]\n"), threadPath.timeTotal);
        }
        return 0;
        case ENUM_MODE::MOVE:
        {
            if(pathObsolete == _T(""))
                _tprintf(_T(" This is not right directory to move\n"));

        }
        break;
        case ENUM_MODE::BRING_BACK:
        {
            if (GetFileAttributes(pathObsolete.c_str()) == INVALID_FILE_ATTRIBUTES)
                _tprintf(_T(" There is no obsolete directory to bring back\n"));

        }
        return 0;
        case ENUM_MODE::BIN:
        {
        }
        return 0;
        case ENUM_MODE::BIN_EMPTY:
        {
            //// Blow Trash Bin
            HRESULT hResult;
            hResult = SHEmptyRecycleBin(NULL, NULL, SHERB_NOPROGRESSUI || SHERB_NOCONFIRMATION);
        }
        break;
        case ENUM_MODE::QUIT:
            return 0;
        default:;
        }
    }
    
    // end loop
    //----------------------------------------------------------------------------------------
    return 0;
}






ENUM_MODE GetMode()
{
    _tprintf(_T("\nselect one (del, move, bring back, bin, bin empty, quit\n"));

    std::string s;
    getline(std::cin, s);
    if (s == "del")
    {
        return ENUM_MODE::DEL;
    }
    if (s == "move")
    {
        return ENUM_MODE::MOVE;
    }
    if (s == "bring back")
    {
        return ENUM_MODE::BRING_BACK;
    }
    if (s == "bin")
    {
        return ENUM_MODE::BIN;
    }
    if (s == "bin empty")
    {
        return ENUM_MODE::BIN_EMPTY;
    }
    if (s == "quit")
    {
        return ENUM_MODE::QUIT;
    }
    if (s == "q")
    {
        return ENUM_MODE::QUIT;
    }
    return ENUM_MODE::NOTHING;
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
        DWORD ftyp = GetFileAttributesA(subPath);
        if (ftyp != INVALID_FILE_ATTRIBUTES && ftyp & FILE_ATTRIBUTE_DIRECTORY)
        {
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
            if (!bHit)
                FindTargetRecursive(pathsHit, TSTRING(subPathS), deletingListFolder, deletingListExtension);
        }
        // File
        else if (ftyp != INVALID_FILE_ATTRIBUTES)
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
