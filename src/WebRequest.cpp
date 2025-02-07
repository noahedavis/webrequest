
#include "WebRequest.h"

WebRequest::WebRequest() {
    this->MAX_NUM_RETRIES = 3;
}

WebResponse WebRequest::request (
                string url,
                const multimap<string, string>& headers,
                const string& data,
                const vector<Authenticator*>& authenticators,
                bool mutual_tls,
                string method
) {
    
    WebResponse response;

    // Set the method for the request
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
            return this->win_http.failure_response;
        }

        // Insert session headers from the authenticators
        multimap<string, string> request_headers;
        for (Authenticator* authenticator : authenticators) {
            if (authenticator) {
                multimap<string, string> authentication_headers = authenticator->getAuthenticationHeaders();
                for (const auto& [name, val] : authentication_headers){
                    request_headers.insert({name, val});
                }
            }
        }

        // Insert request headers
        for (const auto& [name, val] : headers){
            request_headers.insert({name, val});
        }

        // Send the request, and if the request was successful, return the response immediately
        response = this->win_http.request(url, method, mutual_tls, request_headers, data);
        if (response.success){
            return response;
        }
    }

    // Return the request's failure response
    return response;
}

future<WebResponse> WebRequest::requestAsync(string url,
            const multimap<string, string>& headers,
            const string& data,
            const vector<Authenticator*>& authenticators,
            bool mutual_tls,
            string method
) {
    // Send the request using the new WinHttp object
    return async(std::launch::async, &WebRequest::request, this, url, headers, data, authenticators, mutual_tls, method);
}

void WebRequest::setMaxNumRetries(unsigned int num_retries) {
    this->MAX_NUM_RETRIES = num_retries;
}

bool WebRequest::ensureAuthentication(const vector<Authenticator*>& authenticators) {
    for (Authenticator* authenticator : authenticators) {
        if (authenticator) {
            if (authenticator->ensureAuthentication(this->win_http) == false) {
                return false;
            }
        }
    }
    return true;
}

