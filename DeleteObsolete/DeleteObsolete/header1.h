#pragma once
#include <tchar.h>
#include <string>
#include <iostream>

typedef std::basic_string<TCHAR> tstring;

void getline(tstring& str)
{
    std::string line;
    std::getline(std::cin, line);

#ifdef _UNICODE // wstring 이면 변환
    str.assign(line.begin(), line.end());
#else // 아니면 대입
    str = line;
#endif
}