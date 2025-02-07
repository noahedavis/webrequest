#include "WinString.h"

wstring toWstring(LPWSTR source, int len) {

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