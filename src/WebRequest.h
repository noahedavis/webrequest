#ifndef __WEBREQUESTER_H__
#define __WEBREQUESTER_H__

#include <string>
#include <vector>

#include "WinHttp.h"
#include "Authenticator.h"

#include <future>

#define JSON_HEADER {"Content-Type", "application/json; charset=utf-8"}
#define AJAX_HEADER {"X-Requested-With", "XMLHttpRequest"}

using std::future;

using FutureWebResponse = future<WebResponse>;

class WebRequest {

    public:

        WebRequest();

        WebResponse request (
            string url,
            const multimap<string, string>& headers = {},
            const string& data = "",
            const vector<Authenticator*>& authenticators = {},
            bool mutual_tls = false,
            string method = ""
        );

        FutureWebResponse requestAsync(
            string url,
            const multimap<string, string>& headers = {},
            const string& data = "",
            const vector<Authenticator*>& authenticators = {},
            bool mutual_tls = false,
            string method = ""
        );

        void setMaxNumRetries(unsigned int num_retries);

    private:

        WinHttp win_http;

        unsigned int MAX_NUM_RETRIES;

        bool ensureAuthentication(const vector<Authenticator*>& authenticators = {});

};

#endif