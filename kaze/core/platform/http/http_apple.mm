#include "http.h"
#import <Foundation/Foundation.h>
#include <kaze/core/errors.h>
#include <kaze/core/str.h>
#include <kaze/core/io/stream/RstreamableMemory.h>

KAZE_NS_BEGIN

static auto toNSString(CStringView str)
{
    return [[NSString alloc] initWithBytes: str.data()
                                    length: str.length()
                                 encoding: NSUTF8StringEncoding];
}

static auto toNSString(const String &str)
{
    return [[NSString alloc] initWithBytes: str.data()
                                    length: str.length()
                                 encoding: NSUTF8StringEncoding];
}

auto http::sendHttpRequestSync(
    const HttpRequest &req
) -> HttpResponse
{
    @autoreleasepool {
        // Convert StringView -> NSString
        NSString *nsURLString =
            [[NSString alloc] initWithBytes: req.url().data()
                                     length: req.url().length()
                                   encoding: NSUTF8StringEncoding];
        NSURL *url = [NSURL URLWithString: nsURLString];

        NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL: url];

        for (const auto &[header, value] : req.headers())
        {
            [request setValue: toNSString(value)
           forHTTPHeaderField: toNSString(header)];
        }

        const auto method = HttpRequest::getMethodString(req.method());
        request.HTTPMethod = toNSString(method);

        if ( !req.mimeType().empty() )
        {
            [request setValue: toNSString(req.mimeType())
           forHTTPHeaderField: @"Content-Type"];
        }

        if (!req.body().empty())
        {
            [request setValue: [NSString stringWithFormat:@"%zu", req.body().size()]
           forHTTPHeaderField: @"Content-Length"];
            [request setValue: @"Keep-Alive"
           forHTTPHeaderField: @"Connection"];
            NSInputStream *stream = [NSInputStream inputStreamWithData:[[NSData alloc] initWithBytes: req.body().data() length: req.body().length()]];
            [request setHTTPBodyStream: stream];
        }

        // Create URLSession data task
        NSURLSessionConfiguration *config = [NSURLSessionConfiguration defaultSessionConfiguration];
        config.timeoutIntervalForRequest = 30.0;
        config.timeoutIntervalForResource = 60.0;
        NSURLSession *session = [NSURLSession sessionWithConfiguration: config];

        // Create semophore to wait for async task
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);

        __block Int status = -1;
        __block String body;
        __block String errorMsg;
        __block Dictionary<String, String> headers;
        __block List<String> cookies;

        // Create URLSession data task
        NSURLSessionDataTask *task = [session
            dataTaskWithRequest: request
            completionHandler: ^(NSData *data, NSURLResponse *response, NSError *error) {
                if (error)
                {
                    errorMsg = error.localizedDescription.UTF8String;
                }
                else if (response && [response isKindOfClass: [NSHTTPURLResponse class]])
                {
                    NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
                    status = (Int)httpResponse.statusCode;

                    for (NSString *key in httpResponse.allHeaderFields)
                    {
                        NSString *value = httpResponse.allHeaderFields[key];


                        auto keyStr = str::toLower([key UTF8String]);
                        if (keyStr == "set-cookie")
                        {
                            cookies.emplace_back([value UTF8String]);
                        }
                        else
                        {
                            headers[keyStr] = [value UTF8String];
                        }
                    }

                    if (data)
                    {
                        body = String((const char *)data.bytes, data.length);
                    }
                }
                dispatch_semaphore_signal(semaphore);
            }];

        [task resume];

        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);

        return {
            .status = status,
            .body = std::move(body),
            .error = std::move(errorMsg),
            .headers = std::move(headers),
            .cookies = std::move(cookies)
        };
    }
}

auto http::sendHttpRequest(
    const HttpRequest &req,
    funcptr_t<void(const HttpResponse &, void *)> callback,
    void *userptr) -> Bool
{
    @autoreleasepool {
        // Convert StringView -> NSString
        NSString *nsURLString =
            [[NSString alloc] initWithBytes: req.url().data()
                                     length: req.url().length()
                                   encoding: NSUTF8StringEncoding];
        NSURL *url = [NSURL URLWithString: nsURLString];
        if ( !url )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Invalid URL: {}", [nsURLString UTF8String]);
            return False;
        }

        NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL: url];

        for (const auto &[header, value] : req.headers())
        {
            [request setValue: toNSString(value)
           forHTTPHeaderField: toNSString(header)];
        }

        const auto method = req.methodString();
        request.HTTPMethod = toNSString(method);

        if ( !req.mimeType().empty() )
        {
            [request setValue: toNSString(req.mimeType())
           forHTTPHeaderField: @"Content-Type"];
        }

        void(^completionHandler)(NSData *data, NSURLResponse *response, NSError *error) =
            ^(NSData *data, NSURLResponse *response, NSError *error) {
                HttpResponse res{};
                if (error)
                {
                    res.error = error.localizedDescription.UTF8String;
                }
                else if (response && [response isKindOfClass: [NSHTTPURLResponse class]])
                {
                    NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
                    res.status = (Int)httpResponse.statusCode;

                    for (NSString *key in httpResponse.allHeaderFields)
                    {
                        NSString *value = httpResponse.allHeaderFields[key];


                        auto keyStr = str::toLower([key UTF8String]);
                        if (keyStr == "set-cookie")
                        {
                            res.cookies.emplace_back([value UTF8String]);
                        }
                        else
                        {
                            res.headers[keyStr] = [value UTF8String];
                        }
                    }

                    if (data)
                    {
                        res.body = String((const char *)data.bytes, data.length);
                    }
                }

                callback(res, userptr);
            };

        if (!req.body().empty())
        {
            [request setValue: [NSString stringWithFormat:@"%zu", req.body().size()]
           forHTTPHeaderField: @"Content-Length"];
            [request setValue: @"Keep-Alive"
           forHTTPHeaderField: @"Connection"];
            NSInputStream *stream = [NSInputStream inputStreamWithData:[[NSData alloc] initWithBytes: req.body().data() length: req.body().length()]];
            [request setHTTPBodyStream: stream];
        }

        // Create URLSession data task
        NSURLSessionConfiguration *config = [NSURLSessionConfiguration defaultSessionConfiguration];
        config.timeoutIntervalForRequest = 30.0;
        config.timeoutIntervalForResource = 60.0;
        NSURLSession *session = [NSURLSession sessionWithConfiguration: config];
        NSURLSessionDataTask *task = [session dataTaskWithRequest: request
                                                  completionHandler:completionHandler];
        [task resume];

        return True;
    }
}

KAZE_NS_END
