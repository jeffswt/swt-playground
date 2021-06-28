
#ifndef _QUINELLA_HTTP_H
#define _QUINELLA_HTTP_H

#include "tcp.h"


/// Properties that are common to all HTTP datagrams.
class HttpObject {
public:
    /// The first line of the datagram.
    std::string firstline;
    /// A vector of <key, value> pairs of HTTP headers.
    std::vector<std::pair<std::string, std::string>> headers;
    /// HTTP datagram body.
    std::string body;
    /// Gets header value by key.
    /// @param key: Header key.
    /// @return Value irrelevant to capitalization.
    std::string get_header_value(std::string key);
};

/// HTTP request datagram.
class HttpRequest : public HttpObject {
public:
    /// Request method, e.g. "GET".
    std::string method;
    /// Requested relative URL, e.g. "/api".
    std::string url;
    /// Requesting HTTP version, e.g. "HTTP/2.0".
    std::string httpver;
    HttpRequest(HttpObject obj);
};

std::ostream& operator << (std::ostream &out, HttpRequest req);

/// HTTP response datagram.
class HttpResponse : public HttpObject {
public:
    /// HTTP version, e.g. "HTTP/1.1".
    std::string httpver;
    /// Status code, e.g. "404".
    std::string statuscode;
    /// Status description, e.g. "Not Found".
    std::string statustext;
    HttpResponse(HttpObject obj);
};

std::ostream& operator << (std::ostream &out, HttpResponse req);

/// An HTTP connection is consisted of HTTP request datagrams and corresponding
/// HTTP response datagrams. They may not correspond to each other.
class HttpConnection {
public:
    /// Connection endpoints description.
    ConnTuple conn;
    /// HTTP request datagrams.
    std::vector<HttpRequest> requests;
    /// HTTP response datagrams.
    std::vector<HttpResponse> responses;
    HttpConnection();
    HttpConnection(ConnTuple conn);
};

HttpConnection http_conn_from_tcp_stream(TcpStream stream);

#endif  // _QUINELLA_HTTP_H
