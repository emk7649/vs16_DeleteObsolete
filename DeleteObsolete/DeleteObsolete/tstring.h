#pragma once

#include <tchar.h>
#include <string>
#include <iostream>
typedef std::basic_string<TCHAR> tstring;

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

#ifdef _UNICODE // wstring �̸� ��ȯ
    str.assign(line.begin(), line.end());
#else // �ƴϸ� ����
    str = line;
#endif
}