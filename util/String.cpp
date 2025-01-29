#include "String.h"

vector<string> split(string str, string delim) {
    vector<string> vals;

    unsigned int delim_len = delim.length();

    unsigned int start = 0;
    unsigned int end = 0;

    while (end != -1){
        end = str.find(delim, start);
        vals.emplace_back(str.substr(start, end - start));
        start = end + delim_len;
    }

    return vals;
}

bool contains(string str, string targ) {
    return (str.find(targ) != -1);
}

string left(string str, unsigned int num_char) {
    return str.substr(0, num_char);
}

string right(string str, unsigned int num_char) {
    return str.substr((str.length() - 1) - num_char);
}

string ltrim(string str) {
    unsigned int pos = 0;
    unsigned int str_len = str.length();

    while (pos < str_len && isspace(str[pos])) {
        ++pos;
    }

    return str.substr(pos);
}

string join(vector<string> vals, string delim) {
    string joined = "";

    for (string val : vals){
        joined += val + delim;
    }

    return joined;
}

string replace(const string& original, const string& find, const string& replace_with) {
    return std::regex_replace(original, std::regex(find), replace_with);
}

bool isWhitespace(const string& str) {
    for (char ch : str){
        if (!isspace(ch)){
            return false;
        }
    }

    return true;
}

bool isNumeric(const string& str) {
    for (char ch : str){
        if (!isdigit(ch)){
            return false;
        }
    }

    return (!isWhitespace(str));
}