
#include "http.h"

#include "deps/unprettysoup/unprettysoup.h"

using namespace std;
using namespace us3;


string HttpObject::get_header_value(string key) {
    String ukey = String(key).lower();
    for (auto &pr : headers)
        if (String(pr.first).lower() == ukey)
            return pr.second;
    return "";
}

HttpRequest::HttpRequest(HttpObject obj) {
    firstline = obj.firstline;
    headers = obj.headers;
    body = obj.body;
    // parse first line
    auto words_r = String(firstline).split(" ");
    vector<string> words;
    for (auto w : words_r)
        if (w.length() > 0)
            words.push_back(w.to_string());
    // assign components
    if (words.size() > 0)
        method = words[0];
    if (words.size() > 1)
        url = words[1];
    if (words.size() > 2)
        httpver = words[2];
}

ostream& operator << (ostream &out, HttpRequest req) {
    out << "HttpRequest(method=" << req.method;
    out << ", url=" << req.url;
    out << ", httpver=" << req.httpver;
    out << ", headers=";
    visualize_dict(out, req.headers);
    out << ", len(body)=" << req.body.length() << ")";
    return out;
}

HttpResponse::HttpResponse(HttpObject obj) {
    firstline = obj.firstline;
    headers = obj.headers;
    body = obj.body;
    // parse first line
    auto words_r = String(firstline).split(" ");
    vector<String> words;
    for (auto w : words_r)
        if (w.length() > 0)
            words.push_back(w);
    // assign components
    if (words.size() > 0)
        httpver = words[0].to_string();
    if (words.size() > 1)
        statuscode = words[1].to_string();
    if (words.size() > 2) {
        words.erase(words.begin());
        words.erase(words.begin());
        statustext = String(" ").join(words).to_string();
    }
}

ostream& operator << (ostream &out, HttpResponse req) {
    out << "HttpResponse(httpver=" << req.httpver;
    out << ", statuscode=" << req.statuscode;
    out << ", statustext='" << req.statustext;
    out << "', headers=";
    visualize_dict(out, req.headers);
    out << ", len(body)=" << req.body.length() << ")";
    return out;
}

/// Parses single line of header into <key, value> header pair.
/// @param line: Header line, format: "This-Is-A-Header: value".
/// @return <key, value> string pair.
pair<string, string> _http_parse_header(string line) {
    String inp(line);
    vector<String> comps = inp.split(":");
    String key = comps[0];
    comps.erase(comps.begin());
    String value = String(':').join(comps).strip();
    return make_pair(key.to_string(), value.to_string());
}

/// Parses entire TCP stream into list of HTTP datagrams.
/// Not yet discriminating on requests or responses.
/// @param stream: TCP stream in bytes.
/// @return A list of HTTPObject's.
vector<HttpObject> _http_parse_objects(string stream) {
    vector<HttpObject> result;
    string buffer;
    int state = 0;  // line state, 0: regular, 1: cr
    HttpObject current;
    int objstate = 0;  // httpobject state, 0: nothing, 1: first line read
    for (int pos = 0; pos < stream.length(); ) {
        char ch = stream[pos];
        // verdict line on current dfa state
        if (state == 0) {
            // react only on carriage return
            if (ch != '\r')
                buffer += ch;
            else
                state = 1;
            pos += 1;
            continue;  // does not form new line
        } else if (state == 1) {
            // react only on line feed
            if (ch != '\n') {
                buffer += '\r';  // compensate lost cr
                buffer += ch;
                state = 0;
                pos += 1;
                continue;  // continue reading line
            }
            state = 0;
            pos += 1;
        }
        // has new line, see what it does
        if (objstate == 0) {
            // respect only non-empty first lines
            if (buffer.length() > 0) {
                current.firstline = buffer;
                buffer.clear();
                objstate = 1;
                continue;
            }
        } else if (objstate == 1) {
            // process body whenever empty
            if (buffer.length() > 0) {
                current.headers.push_back(_http_parse_header(buffer));
                buffer.clear();
                continue;
            }
            objstate = 0;
        }
        // now process body, find content-length
        string s_len = current.get_header_value("content-length");
        int contlen = string_to_int(s_len);
        if (contlen == -1)
            contlen = stream.length();
        for (int i = 0; i < contlen && pos < stream.length(); i++, pos++)
            current.body += stream[pos];
        // add to results and clear
        result.push_back(current);
        current.firstline.clear();
        current.headers.clear();
        current.body.clear();
        objstate = 0;
        buffer.clear();
        state = 0;
    }
    return result;
}

HttpConnection::HttpConnection() {}

HttpConnection::HttpConnection(ConnTuple conn) : conn(conn) {}

HttpConnection http_conn_from_tcp_stream(TcpStream stream) {
    HttpConnection result(stream.conn);
    vector<HttpObject> req = _http_parse_objects(stream.outbound);
    vector<HttpObject> resp = _http_parse_objects(stream.inbound);
    // convert objects into their respective subtypes
    for (auto i : req)
        result.requests.push_back(HttpRequest(i));
    for (auto i : resp)
        result.responses.push_back(HttpResponse(i));
    return result;
}
