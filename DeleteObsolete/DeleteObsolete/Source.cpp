#include <cstdio>
#include <tchar.h>
#include <windows.h>

#include <conio.h>
#include <locale.h>

#include <string>
#include <iostream>
#include "header1.h"

long g_nMode; // 0:Del(default),  1:Bin,  2:Move
int _tmain(int argc, TCHAR* argv[])
{
    _tsetlocale(LC_ALL, _T("korean"));
    tstring s;
    getline(s);

    int size_s = s.length();

    _tprintf(s.c_str());


    return 0;
}