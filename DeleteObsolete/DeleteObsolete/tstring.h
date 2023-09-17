#pragma once

#include <tchar.h>
#include <string>
#include <iostream>
typedef std::basic_string<TCHAR> tstring;

template <typename T>
tstring TSTRING(T& t)
{
    tstring str;
    str.assign(t.begin(), t.end());
    return str;
}

std::string STRING(tstring& t)
{
    std::string str;
    str.assign(t.begin(), t.end());
    return str;
}

std::wstring WSTRING(tstring& t)
{
    std::wstring str;
    str.assign(t.begin(), t.end());
    return str;
}

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