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
    mbstowcs_s(&num, pszWchar, t.length() + 1, t.c_str(), _TRUNCATE);  // CHAR -> WCHAR 변환
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
    wcstombs_s(&num, pszWchar, size, t.c_str(), _TRUNCATE);  // WCHAR -> CHAR 변환
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
    wcstombs_s(&num, pszWchar, size, t.c_str(), _TRUNCATE);  // WCHAR -> CHAR 변환
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
    mbstowcs_s(&num, pszWchar, t.length() + 1, t.c_str(), _TRUNCATE);  // CHAR -> WCHAR 변환
    str = pszWchar;
    delete[] pszWchar;
#endif
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

/*
* wstring의 length()는 wchar사이즈(2바이트)로 센다.
* string의 length()는 mbs라서 askii(1바이트), 기타(2바이트)로 askii 이외 문자가 섞여 있으면 length()와 문자의 수가 맞지 않는다.
* wstring to string에서 mbstowcs_s 사용 시, string의 length() + 1(NULL 포함)    이 아닌       (string의 length() + 1(NULL 포함)) * 2 사용
* wstring을 변환한 string의 length는 기존 wstring의 length 대비 최대 2배 늘어나므로
*/