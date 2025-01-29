#include "Authenticator.h"

using std::lock_guard;

bool Authenticator::ensureAuthentication() {

    lock_guard<mutex> lock(authen_mtx);

    for (this->curr_try = 0; this->curr_try < this->MAX_NUM_TRIES; this->curr_try++){
        if (this->authenticate()){
            this->is_authenticated = true;
            return true;
        }
    }

    this->is_authenticated = false;
    return false;
}

WinHttp* Authenticator::getWinHttp(){
    return this->win_http;
}

bool Authenticator::isAuthenticated() {
    return this->is_authenticated;
}

multimap<string, string> Authenticator::getAuthenticationHeaders() {

    if (this->is_authenticated) {
        return this->authentication_headers;
    }
    return multimap<string, string>();
}

void Authenticator::initializeNumTries(int max_num_tries) {

    if ((max_num_tries > 0) && (max_num_tries < 10)){
        this->MAX_NUM_TRIES = max_num_tries;
        return;
    }

    this->MAX_NUM_TRIES = DEFAULT_MAX_NUM_RETRIES ;
}

Authenticator::Authenticator(WinHttp* win_http, int max_num_tries) {

    this->is_authenticated = false;
    this->initializeNumTries(max_num_tries);
    
    if (win_http) {
        this->win_http = win_http;
    }
    else {
        this->win_http = new WinHttp();
    }

    this->username = wStringToString(this->win_http->getUserName());

}

Authenticator::~Authenticator() {

    lock_guard<mutex> lock(authen_mtx);

    if (this->win_http){
        delete this->win_http;
    }
}
