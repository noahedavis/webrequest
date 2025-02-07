#ifndef __CUSTOM_STRING_H__
#define __CUSTOM_STRING_H__

/**
 * A collection of useful string functions.
 * 
 * Really just to make C++ feel a little more homey
 * 
 */

#include <vector>
#include <string>

#include <regex>

#include <locale>
#include <codecvt>

using std::vector;
using std::string;
using std::wstring;

vector<string> split(string str, string delim);
bool contains(string str, string targ);
string left(string str, unsigned int num_char);
string ltrim(string str);
string join(vector<string> vals, string delim = " ");
string replace(const string& original, const string& find, const string& replace_with);

bool isWhitespace(const string& str);
bool isNumeric(const string& str);

wstring stringToWString(string str);

string wStringToString(wstring str);


#endif // __CUSTOM_STRING_H__