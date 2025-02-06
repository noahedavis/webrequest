#include "Authenticator.h"

using std::lock_guard;

bool Authenticator::ensureAuthentication(WinHttp* win_http) {

    lock_guard<mutex> lock(authen_mtx);

    for (this->curr_try = 0; this->curr_try < this->MAX_NUM_TRIES; this->curr_try++){
        if (this->authenticate(win_http)){
            this->is_authenticated = true;
            return true;
        }
    }

    this->is_authenticated = false;
    return false;
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

void Authenticator::setNumTries(unsigned int max_num_tries) {

    if ((max_num_tries > 0) && (max_num_tries < 10)){
        this->MAX_NUM_TRIES = max_num_tries;
        return;
    }

    this->MAX_NUM_TRIES = DEFAULT_MAX_NUM_RETRIES ;
}

Authenticator::Authenticator(unsigned int max_num_tries) {

    this->is_authenticated = false;
    this->setNumTries(max_num_tries);

}

Authenticator::~Authenticator() {}
