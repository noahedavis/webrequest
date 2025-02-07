#include "../src/WebRequest.h"

WebRequest req;

const string google_url = "https://www.google.com/search?q=C++";

void BasicExample() {

    println("Hello! This is the basic example for the WebRequester class");

    // Send the web request
    WebResponse google_resp = req.request(google_url);

    // Print the response text of the request
    println("Here's the response from the requested url, \"" + google_url + "\"");
    println(google_resp.response_str); 

}

void AsyncExample() {

    println("Hello! This is the async example for the WebRequester class");

    // Send the web request
    FutureWebResponse google_resp_fut = req.requestAsync(google_url);

    // While waiting for the request to execute, do something
    for (int i = 0; i < 20; i++) {
        println("Doing something...");
    }

    // Receive the response
    WebResponse google_resp = google_resp_fut.get();
    if (google_resp.success == true) {
        // Print the response text of the request
        println("Here's the response from the requested url, \"" + google_url + "\"");
        println(google_resp.response_str);
    }
    else {
        // Print a failure message, along with each of the response's headers
        println("Request to \"" + google_url + "\" was unsuccessful!");
        for (auto const & [header_name, header_value] : google_resp.headers) {
            println("Response header: \"" + header_name + ": " + header_value);
        }
    }

    println("Done!");
}

class DummyAuthenticator : public Authenticator {

    public:

        DummyAuthenticator() {}

        bool authenticate(WinHttp* win_http) {

            if (win_http == NULL) {
                return false;
            }

            // Take some series of steps to perform authentication ...
            multimap<string, string> auth_headers;
            auth_headers.insert({"X-Csrf-Token", "tok"});

            WebResponse auth_resp = win_http->request("https://dummy-site.com?username=username&password=password", "GET", false, auth_headers);

            // If the authentication is successful, save the authentication response headers
            if (auth_resp.success) {
                this->authentication_headers.insert({"Secret", auth_resp.headers["Secret"]});
                println("Authentication successful!");
                return true;
            }

            println("Authentication failed!");
            return false;
        }
};

void AuthenticatorExample() {

    DummyAuthenticator dummy;
    println("Hello! This is the authenticator example for the WebRequester class");

    WebResponse dummy_authenticated_resp = req.request(google_url, {}, "", {&dummy});
    if (dummy_authenticated_resp.success == true) {

        // Print the response text of the request
        println("Here's the response from the requested url, \"" + google_url + "\"");
        println(dummy_authenticated_resp.response_str); 
    }
    else {
        // Print a failure message, along with each of the response's headers
        println("Request to \"" + google_url + "\" was unsuccessful!");
        for (auto const & [header_name, header_value] : dummy_authenticated_resp.headers) { // Print the response headers
            println("Response header: \"" + header_name + "= " + header_value);
        }
    }

    println("Done!");

}

int main() {
    BasicExample();
    AsyncExample();
    AuthenticatorExample();
    return 0;
}