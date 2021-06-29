
#include <map>
#include "quinella.h"
#include "deps/unprettysoup/unprettysoup.h"

using namespace std;
using namespace us3;


/// Parse URLencoded string into Unicode.
/// @param input: Input string, e.g. '%01%02abc%23'.
/// @return Unicode string.
String urlparse(String input) {
    string inp = input.to_string();
    string result;
    for (int i = 0; i < inp.length(); ) {
        char ch = inp[i];
        if (ch == '%') {
            int a = hex_to_int(inp[i + 1]),
                b = hex_to_int(inp[i + 2]);
            result += (char)(a * 16 + b);
            i += 3;
        } else {
            result += ch;
            i += 1;
        }
    }
    return result;
}

/// Parse www-form-urlencoded into map.
map<String, String> parse_qs(String body) {
    map<String, String> result;
    for (auto pr : body.split("&")) {
        auto x = pr.split("=");
        result[urlparse(x[0])] = urlparse(x[1]);
    }
    return result;
}

void foo(TcpStream stream) {
    // cout << stream.conn << endl;
    // TlsStream tc = tls_stream_from_tcp_stream(stream);
    // cout << tc.conn << endl;
    // cout << b16encode(tc.client_random) << endl;
    // cout << b16encode(tc.server_random) << endl;
    // cout << b16encode(tc.master_secret) << endl;
    if (stream.conn.second.port != 80)
        return ;  // skip non http
    HttpConnection conn = http_conn_from_tcp_stream(stream);
    for (auto &req : conn.requests) {
        auto form = parse_qs(req.body);
        // check if is mail send
    }
    return ;
}

int main() {
    tls_set_keylogfile_path("./keylog.txt");
    auto devices = pcap_get_all_devices();
    for (auto device : devices) {
        if (device.name() != "eth0")
            continue;
        cout << "Running capture on device '" << device.name() << "'\n";
        tcp_capture_stream(device, "", foo);
        break;
    }
    return 0;
}
