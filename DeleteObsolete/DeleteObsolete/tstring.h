#pragma once

#include <tchar.h>
#include <string>
#include <iostream>
#include <format>
typedef std::basic_string<TCHAR> tstring;

tstring TSTRING(std::string t)
{
    tstring str;
#ifdef _UNICODE
    wchar_t* pszWchar = new wchar_t[t.length() + 1];
    size_t num;
    mbstowcs_s(&num, pszWchar, t.length() + 1, t.c_str(), _TRUNCATE);  // CHAR -> WCHAR ��ȯ
    str = pszWchar;
    delete[] pszWchar;
#else
    str.assign(t.begin(), t.end());
#endif
    return str;
}

tstring TSTRING(std::wstring t)
{
    tstring str;
#ifdef _UNICODE
    str.assign(t.begin(), t.end());
#else
    int size = (t.length() + 1) * 2;
    char* pszWchar = new char[size];
    size_t num;
    wcstombs_s(&num, pszWchar, size, t.c_str(), _TRUNCATE);  // WCHAR -> CHAR ��ȯ
    str = pszWchar;
    delete[] pszWchar;
#endif
    return str;
}

std::string STRING(tstring& t)
{
    std::string str;
#ifdef _UNICODE
    int size = (t.length() + 1) * 2;
    char* pszWchar = new char[size];
    size_t num;
    wcstombs_s(&num, pszWchar, size, t.c_str(), _TRUNCATE);  // WCHAR -> CHAR ��ȯ
    str = pszWchar;
    delete[] pszWchar;
#else
    str.assign(t.begin(), t.end());
#endif
    return str;
}

std::wstring WSTRING(tstring& t)
{
    std::wstring str;
#ifdef _UNICODE
    str.assign(t.begin(), t.end());
#else
    wchar_t* pszWchar = new wchar_t[t.length() + 1];
    size_t num;
    mbstowcs_s(&num, pszWchar, t.length() + 1, t.c_str(), _TRUNCATE);  // CHAR -> WCHAR ��ȯ
    str = pszWchar;
    delete[] pszWchar;
#endif
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

/*
* wstring�� length()�� wchar������(2����Ʈ)�� ����.
* string�� length()�� mbs�� askii(1����Ʈ), ��Ÿ(2����Ʈ)�� askii �̿� ���ڰ� ���� ������ length()�� ������ ���� ���� �ʴ´�.
* wstring to string���� mbstowcs_s ��� ��, string�� length() + 1(NULL ����)    �� �ƴ�       (string�� length() + 1(NULL ����)) * 2 ���
* wstring�� ��ȯ�� string�� length�� ���� wstring�� length ��� �ִ� 2�� �þ�Ƿ�
*/