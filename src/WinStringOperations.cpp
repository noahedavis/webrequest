#include "WinStringOperations.h"
#include <windows.h>

wstring stringToWString(string str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
}

string wStringToString(wstring str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(str);
}

wstring substringLPWSTR(LPWSTR source, int len) {
    wstring wSource(source);
    wstring substr = wSource.substr(0, len);
    return substr;
}

wstring getUserName() {

    const int MAX_UN_LEN = 256;
    WCHAR username_buffer[MAX_UN_LEN];
    unsigned long size = MAX_UN_LEN;

    if (GetUserNameW(username_buffer, &size)) {
        return wstring(username_buffer);
    }

    return L"";
}