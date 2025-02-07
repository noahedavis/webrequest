#include "WinHttp.h"

using std::lock_guard;

WinHttp::WinHttp() {
    this->newSession();
    this->initializeUserName();
    this->getClientCertificates();
}

WinHttp::~WinHttp() {
    this->closeConnections();
    this->closeSession();
}

bool WinHttp::addHeader(const HINTERNET& req, const string& name, const string& value) {
    string header = name + ": " + value;
    wstring wHeader(header.begin(), header.end());
    
    return WinHttpAddRequestHeaders(req, wHeader.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD);
}

bool WinHttp::addHeaders(const HINTERNET& req, multimap<string, string> headers) {
    for (auto const& [name, val] : headers){
        if (!addHeader(req, name, val)) {
            return false;
        }
    }

    return true;
}

WebResponse WinHttp::request(const string& url, const string& method, bool mutual_tls, multimap<string, string> headers, const string& body) {

    UrlComponents components = getUrlComponents(url);

    HINTERNET conn = this->getServerConnection(components.host, components.port);
    if (conn == NULL){
        return this->failure_response;
    }

    wstring wMethod = toWString(method);

    HINTERNET req;
    if (isSecureUrl(url)) {
        req =  WinHttpOpenRequest(conn, wMethod.c_str(), components.path.c_str(), L"HTTP/2", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    } else {
        req =  WinHttpOpenRequest(conn, wMethod.c_str(), components.path.c_str(), L"HTTP/2", WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    }

    bool disabled_timeouts = this->disableTimeouts(req);
    bool allowed_cookies = this->allowCookies(req);
    bool auto_decompressing = this->enableDecompression(req);
    bool set_autologin;
    bool set_client_certificates;

    if (mutual_tls){
        set_autologin = this->setAutoLoginPolicy(req);
        set_client_certificates = this->setClientCertificate(req);
    }

    this->addHeaders(req, headers);

    int data_len = body.length();
    TCHAR data_buff[data_len];
    for (int i = 0; i < data_len; i++) data_buff[i] = body[i];

    bool request_sent = WinHttpSendRequest(req, WINHTTP_NO_ADDITIONAL_HEADERS, 0, data_buff, data_len, data_len, 0);
    if (!request_sent){
        return this->failure_response;
    }

    bool response_received = WinHttpReceiveResponse(req, 0);
    if (!response_received){
        return this->failure_response;
    }

    int status_code = this->receiveStatusCode(req);
    map<string, string> response_headers = this->receiveResponseHeaders(req, components.host);
    string response_str;
    vector<unsigned char> response_bytes;

    response_str = this->receiveResponseStr(req);
    
    return {response_str, status_code, (status_code == 200), response_headers};

}

void WinHttp::initializeUserName() {
    this->user_name = getUserName();
}

bool WinHttp::isSecureUrl(const string& url) {
    return (left(url, 6) == "https:");
}

bool WinHttp::getClientCertificates() {

    HCERTSTORE store = CertOpenSystemStore(0, TEXT("MY"));

    if (store) {
        this->client_certificates = CertFindCertificateInStore(
            store, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_SUBJECT_STR, this->user_name.c_str(), 0
        );
    }

    CertCloseStore(store, 0);

    if (this->client_certificates) return true;
    return false;
}

bool WinHttp::newSession() {
    this->session = WinHttpOpen(L"WinHttp", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    unsigned int redirect_policy = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
    
    if (WinHttpSetOption(this->session, WINHTTP_OPTION_REDIRECT_POLICY, &redirect_policy, sizeof(redirect_policy))) {
        return true;
    }
    return false;
}

HINTERNET WinHttp::newServerConnection(const wstring& host, const int& port) {
    HINTERNET conn = WinHttpConnect(this->session, host.c_str(), port, 0);

    if (conn) return conn;
    return NULL;
}

WinHttp::UrlComponents WinHttp::getUrlComponents(const string& url) {
    URL_COMPONENTS comp;
    ZeroMemory(&comp, sizeof(comp));

    comp.dwStructSize = sizeof(comp);

    comp.dwSchemeLength = -1;
    comp.dwHostNameLength = -1;
    comp.dwUrlPathLength = -1;
    comp.dwExtraInfoLength = -1;

    wstring wurl = toWString(url);
    WinHttpCrackUrl(wurl.c_str(), 0, 0, &comp);

    wstring host = getHostFromUrlComponents(&comp);
    wstring path = getPathFromUrlComponents(&comp);
    INTERNET_PORT port = getPortFromUrlComponents(&comp);

    return {path, host, port};
}

wstring WinHttp::getHostFromUrlComponents(URL_COMPONENTS * components) {
    return toWstring(
        components->lpszHostName,
        components->dwHostNameLength
    );
}

INTERNET_PORT WinHttp::getPortFromUrlComponents(URL_COMPONENTS * components) {
    return components->nPort;
}

wstring WinHttp::getPathFromUrlComponents(URL_COMPONENTS * components) {
    return toWstring(
        components->lpszUrlPath,
        components->dwUrlPathLength
    ) 
    + toWstring(
        components->lpszExtraInfo,
        components->dwExtraInfoLength
    );
}

HINTERNET WinHttp::getServerConnection(const wstring& host, const INTERNET_PORT& port) {

    lock_guard<mutex> guard(conn_mtx);

    if (server_connections[host] == NULL){
        server_connections[host] = this->newServerConnection(host, port);
    }

    return server_connections[host];
}

bool WinHttp::allowCookies(const HINTERNET& req) {
    return WinHttpAddRequestHeaders(req, L"Cookie:", -1, WINHTTP_ADDREQ_FLAG_REPLACE);
}

bool WinHttp::setAutoLoginPolicy(const HINTERNET& req) {
    unsigned int logon_policy = WINHTTP_AUTOLOGON_SECURITY_LEVEL_LOW;
    if (WinHttpSetOption(req, WINHTTP_OPTION_AUTOLOGON_POLICY, &logon_policy, sizeof(logon_policy))){
        return true;
    }
    return false;
}

bool WinHttp::disableTimeouts(const HINTERNET& req) {
    return WinHttpSetTimeouts(req, -1, -1, -1, -1);
}

bool WinHttp::setClientCertificate(const HINTERNET& req) {
    return WinHttpSetOption(req, WINHTTP_OPTION_CLIENT_CERT_CONTEXT, (LPVOID) this->client_certificates, sizeof(CERT_CONTEXT));
}

bool WinHttp::enableDecompression(const HINTERNET& req) {
    unsigned int decompression_setting = WINHTTP_DECOMPRESSION_FLAG_ALL;
    return WinHttpSetOption(req, WINHTTP_OPTION_DECOMPRESSION, &decompression_setting, sizeof(decompression_setting));
}

int WinHttp::receiveStatusCode(const HINTERNET& req) {
    int status_code = -1;

    unsigned long size = sizeof(status_code);

    WinHttpQueryHeaders (
        req,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &status_code,
        &size,
        WINHTTP_NO_HEADER_INDEX
    );

    return status_code;
}

vector<unsigned char> WinHttp::receiveResponseBytes(const HINTERNET& req) {

    unsigned long buffer_size = 0;
    unsigned long bytes_downloaded = 0;

    vector<unsigned char> response_bytes;

    do {

        if (!WinHttpQueryDataAvailable(req, &buffer_size)) {
            return vector<unsigned char>();
        }

        unsigned char buffer[buffer_size + 1];
        ZeroMemory(buffer, buffer_size + 1);
        unsigned int buffer_len = sizeof(buffer) / sizeof(buffer[0]);

        if (!WinHttpReadData(req, buffer, buffer_size, &bytes_downloaded)) {
            return vector<unsigned char>();
        }

        copy(buffer, buffer + buffer_len, back_inserter(response_bytes));

    } while (buffer_size > 0);

    return response_bytes;
}

string WinHttp::receiveResponseStr(const HINTERNET& req) {
    unsigned long buffer_size = 0;
    unsigned long bytes_downloaded = 0;

    string response_str = "";

    do {

        if (!WinHttpQueryDataAvailable(req, &buffer_size)) {
            return "";
        }

        char* buffer = new char[buffer_size + 1];

        ZeroMemory(buffer, buffer_size + 1);

        if (!WinHttpReadData(req, buffer, buffer_size, &bytes_downloaded)) {
            return "";
        }

        response_str.append(buffer);

        delete[] buffer;

    } while (buffer_size > 0);

    return response_str;
}

map<string, string> WinHttp::receiveResponseHeaders(const HINTERNET& req, const wstring& host) {

    unsigned long header_size;

    map<string, string> response_headers;

    WinHttpQueryHeaders(req, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &header_size, WINHTTP_NO_HEADER_INDEX);

    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER){

        unsigned long actual_header_size = header_size / sizeof(WCHAR);
        WCHAR* header_buffer = new WCHAR[actual_header_size];

        if (!WinHttpQueryHeaders(req, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, header_buffer, &header_size, WINHTTP_NO_HEADER_INDEX)){
            return response_headers;
        }

        wstring header_wstr(header_buffer);
        string header_str = toString(header_wstr);

        vector<string> header_split = split(header_str, "\r\n");
        for (string header : header_split){
            vector<string> vals = split(header, ": ");
            unsigned int vals_len = vals.size();

            if (vals_len == 2){
                if (response_headers[vals[0]] == "") {
                    response_headers[vals[0]] = vals[1];
                }
                else {
                    response_headers[vals[0]] = response_headers[vals[0]] + "; " + vals[1];
                }
            }
        }

        // Remove the connection from the connection map if its been closed
        string conn_status = response_headers["Connection"];
        if (conn_status == "close"){
            this->removeConnection(host);
        }

    }

    return response_headers;
}

bool WinHttp::closeRequest(const HINTERNET& req) {
    return WinHttpCloseHandle(req);
}

void WinHttp::closeConnections() {
    for (auto const& [server, conn] : this->server_connections) {
        WinHttpCloseHandle(conn);
    }
}

void WinHttp::closeSession() {
    WinHttpCloseHandle(this->session);
}

void WinHttp::removeConnection(const wstring& host) {
    lock_guard<mutex> guard(this->conn_mtx);
    WinHttpCloseHandle(this->server_connections[host]);
    this->server_connections[host] = NULL;
}
