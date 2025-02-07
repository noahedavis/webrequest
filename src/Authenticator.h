#ifndef __AUTHENTICATOR_H__
#define __AUTHENTICATOR_H__

#include "WinHttp.h"
#include <time.h>
#include <mutex>

#define DEFAULT_MAX_NUM_RETRIES 2

class Authenticator {

    public:

        Authenticator(unsigned int max_num_tries = DEFAULT_MAX_NUM_RETRIES);
        ~Authenticator();
        
        bool ensureAuthentication(const WinHttp& win_http);
        bool isAuthenticated();

        multimap<string, string> getAuthenticationHeaders();

    protected:

        bool is_authenticated;
        multimap<string, string> authentication_headers;

        unsigned int MAX_NUM_TRIES;
        unsigned int curr_try;

        void setNumTries(unsigned int max_num_tries = DEFAULT_MAX_NUM_RETRIES);
        virtual bool authenticate(const WinHttp& win_http) = 0;

    private:

        mutex authen_mtx;

};
#endif // __AUTHENTICATOR_H__