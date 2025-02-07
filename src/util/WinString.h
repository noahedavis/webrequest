#ifndef __STRINGOPERATIONS_H__
#define __STRINGOPERATIONS_H__

#include <string>
#include <windows.h>

using std::wstring;
using std::string;

wstring toWstring(LPWSTR source, int len);

wstring getUserName();

#endif // __STRINGOPERATIONS_H__