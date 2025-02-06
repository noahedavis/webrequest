#ifndef __STRINGOPERATIONS_H__
#define __STRINGOPERATIONS_H__

#include <locale>
#include <codecvt>
#include <string>
#include <windows.h>

using std::wstring;
using std::string;

wstring stringToWString(string str);

string wStringToString(wstring str);

wstring substringLPWSTR(LPWSTR source, int len);

wstring getUserName();

#endif // __STRINGOPERATIONS_H__