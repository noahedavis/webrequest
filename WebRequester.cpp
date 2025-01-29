
#include "WebRequester.h"

WebRequester::WebRequester() {
    this->MAX_NUM_RETRIES = 3;
}

WebResponse WebRequester::request (
                string url,
                const multimap<string, string>& headers,
                const string& data,
                const vector<Authenticator*>& authenticators,
                bool mutual_tls,
                WinHttp* request_win_http,
                string method
) {
    
    WebResponse response;

    // Set the WinHttp object to be used for the request. If null, use the default object contained in the class
    if (!request_win_http) {
        request_win_http = &(this->win_http);
    }

    if (method == "") {
        if (data.length() > 0) {
            method = "POST";
        }
        else {
            method = "GET";
        }
    }

    // Attempt to send the request up to MAX_NUM_RETRIES times
    for (int try_num = 0; try_num < this->MAX_NUM_RETRIES; try_num++) {

        // Ensure the web session has been properly authenticated before performing the request
        bool authentication_ensured = this->ensureAuthentication(authenticators);
        if (!authentication_ensured) {
            return request_win_http->failure_response;
        }

        multimap<string, string> request_headers;
        for (Authenticator* authenticator : authenticators) {
            if (authenticator) {
                multimap<string, string> authentication_headers = authenticator->getAuthenticationHeaders();
                for (const auto& [name, val] : authentication_headers){
                    request_headers.insert({name, val});
                }
            }
        }

        for (const auto& [name, val] : headers){
            request_headers.insert({name, val});
        }

        // Send the request, and if the request was successful, return the response immediately
        response = request_win_http->request(url, method, mutual_tls, request_headers, data);
        if (response.success){
            return response;
        }
    }

    // Return the request's failure response
    return response;
}

future<WebResponse> WebRequester::requestAsync(string url,
            const multimap<string, string>& headers,
            const string& data,
            const vector<Authenticator*>& authenticators,
            bool mutual_tls,
            WinHttp* win_http
) {

    // Send the request using the new WinHttp object
    return async(std::launch::async, &WebRequester::request, this, url, headers, data, authenticators, mutual_tls, win_http, "");

}

void WebRequester::setMaxNumRetries(unsigned int num_retries) {
    this->MAX_NUM_RETRIES = num_retries;
}

WinHttp* WebRequester::getWinHttp() {
    return &(this->win_http);
}

bool WebRequester::ensureAuthentication(const vector<Authenticator*>& authenticators) const {
    for (Authenticator* authenticator : authenticators) {
        if (authenticator) {
            if (!authenticator->ensureAuthentication()) {
                return false;
            }
        }
    }
    return true;
}

