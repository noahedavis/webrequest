# webrequester

Simple C++ WinHttp wrapper for creating synchronous/asynchronous web requests. Includes easy customization options for mutual tls, retries and authentication.

## Examples

To send a web request and get it's response, call the request() method on a WebRequester object:

```C++
WebRequester req;

WebResponse resp = req.request("https://www.github.com");
println(resp.response_str);
```


To send an async web request, use the requestAsync() method. After the request is sent, call the get() method on the returned FutureWebResponse to get the results.

```C++
WebRequester req;

FutureWebResponse resp = req.requestAsync("https://www.github.com");

// Do something ...

println(resp.get().response_str);
```

Other basic examples can be found in the example folder

## Overview

The WinHttp class includes the bare bone features needed to send http requests. This class behaves similarly to the WinHttpRequest class in VBA, but no data on the requests is stored within the class instance itself: request data and response data will exist in the methods that call the WinHttp request method

The WebRequester class is the main class, built over the WinHttp class. This class includes options for specifying number of retries on failure and performing custom-defined authentication procedures.

## Build

This project currently supports C++11 through C++ 20, and requires the Windows SDK.

When building the project, you'll need to include each of the cpp files, and these libraries from the Windows SDK:

    winhttp.lib
    Crypt32.Lib
    RpcRT4.Lib

More information on the Windows SDK and API can be found [here](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/) and [here](https://learn.microsoft.com/en-us/windows/win32/apiindex/windows-api-list):
   
