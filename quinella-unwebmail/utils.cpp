
#include "utils.h"

#include <iomanip>


CapException::CapException(std::string what) {
    this->_what = what;
}

const char* CapException::what() const throw() {
    return this->_what.c_str();
}

std::string b16encode(std::string in) {
    static char charmap[17] = "0123456789abcdef";
    std::string out;
    for (char chr : in) {
        out += charmap[(chr >> 4) & 0xf];
        out += charmap[chr & 0xf];
    }
    return out;
}

// hex_to_int() -- Convert hexadecimal character to integer counterpart.
// @param ch: Input character, range = 0-9, a-f, A-F.
// @return Integer counterpart, 0 if the input is invalid.
int hex_to_int(char ch) {
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    return 0;
}

std::string b16decode(std::string in) {
    std::string out;
    for (int i = 0; i < in.length(); i += 2) {
        char a = in[i], b = in[i + 1];
        int va = hex_to_int(a), vb = hex_to_int(b);
        out += (char)(va * 16 + vb);
    }
    return out;
}

int string_to_int(std::string in) {
    if (in.length() == 0)
        return -1;
    int res = 0;
    for (char ch : in)
        if (ch >= '0' && ch <= '9')
            res = res * 10 + ch - '0';
        else
            return -1;
    return res;
}

void print_hex_box(std::ostream &out, std::string msg) {
    for (int i = 0; i < msg.length(); i++) {
        if (i % 32 == 0)
            out << "     ";
        else if (i % 32 == 16)
            out << "   ";
        else
            out << " ";
        int val = (int)msg[i] & 0xff;
        out << std::setfill('0') << std::setw(2) << std::hex << val;
        if (i % 32 == 31 || i + 1 == msg.length())
            out << "\n";
    }
}
