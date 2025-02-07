#ifndef __WinHttp_H__
#define __WinHttp_H__

#include <mutex>
#include "util/Print.h"
#include "util/String.h"

#include <map>
#include <algorithm>

#include <vector>

#include "util/WinString.h"
#include <winhttp.h>

using std::string;
using std::wstring;
using std::map;
using std::vector;
using std::multimap;
using std::mutex;

struct WebResponse {
    string response_str;
    int status_code;
    bool success;
    map<string, string> headers;
};

class WinHttp {

    public:

        struct UrlComponents {
            wstring path;
            wstring host;
            INTERNET_PORT port;
        };

        WinHttp();
        ~WinHttp();
        
        WebResponse request(
                const string& url, 
                const string& method = "GET", 
                bool mutual_tls = false, 
                multimap<string, string> headers = {}, 
                const string& body = ""
        );
        bool addHeader(const HINTERNET& req, const string& name, const string& value);
        bool addHeaders(const HINTERNET& req, multimap<string, string> headers);

        const WebResponse failure_response =  {"", -1, false};

    private:
        
        HINTERNET session;
        PCCERT_CONTEXT client_certificates;

        wstring user_name;

        map<wstring, HINTERNET> server_connections;
        mutex conn_mtx;

        bool isSecureUrl(const string& url);

        bool getClientCertificates();

        bool newSession();
        HINTERNET newServerConnection(const wstring& host, const int& port);


        UrlComponents getUrlComponents(const string& url);
        wstring getHostFromUrlComponents(URL_COMPONENTS * components);
        INTERNET_PORT getPortFromUrlComponents(URL_COMPONENTS * components);
        wstring getPathFromUrlComponents(URL_COMPONENTS * components);

        HINTERNET getServerConnection(const wstring& host, const INTERNET_PORT& port);

        bool allowCookies(const HINTERNET& req);

        bool setAutoLoginPolicy(const HINTERNET& req);
        bool disableTimeouts(const HINTERNET& req);
        bool setClientCertificate(const HINTERNET& req);
        bool enableDecompression(const HINTERNET& req);

        int receiveStatusCode(const HINTERNET& req);
        string receiveResponseStr(const HINTERNET& req);
        map<string, string> receiveResponseHeaders(const HINTERNET& req, const wstring& host);
        vector<unsigned char> receiveResponseBytes(const HINTERNET& req);

        bool closeRequest(const HINTERNET& req);
        void closeConnections();
        void closeSession();

        void removeConnection(const wstring& host);
        void initializeUserName();
};

#endif