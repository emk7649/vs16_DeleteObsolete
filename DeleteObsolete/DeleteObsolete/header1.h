#pragma once
#include <tchar.h>
#include <string>
#include <iostream>

typedef std::basic_string<TCHAR> tstring;

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