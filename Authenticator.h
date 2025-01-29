#ifndef __AUTHENTICATOR_H__
#define __AUTHENTICATOR_H__

#include "WinHttp.h"
#include <time.h>
#include <mutex>

#define DEFAULT_MAX_NUM_RETRIES 2

class Authenticator {

    public:

        Authenticator(WinHttp* win_http = NULL, int max_num_tries = DEFAULT_MAX_NUM_RETRIES);
        ~Authenticator();
        
        bool ensureAuthentication();
        bool isAuthenticated();

        multimap<string, string> getAuthenticationHeaders();
        WinHttp* getWinHttp();

    protected:

        bool is_authenticated;
        multimap<string, string> authentication_headers;
        string username;

        WinHttp* win_http;
        int MAX_NUM_TRIES;
        int curr_try;

        void initializeNumTries(int max_num_tries = DEFAULT_MAX_NUM_RETRIES);
        virtual bool authenticate() = 0;

    private:

        mutex authen_mtx;

};
#endif // __AUTHENTICATOR_H__