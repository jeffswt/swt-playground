
#include <iostream>
#include <vector>
#include <map>


/// Quinella runtime exception.
class CapException : public std::exception {
private:
    std::string _what;
public:
    CapException(std::string what);
    const char* what() const throw();
};

/// Encodes binary string into hexadecimal format.
/// @param in: arbitrary binary string
/// @return hexadecimal string consisting only of 0-9 and a-f,
///         length guaranteed to be even
std::string b16encode(std::string in);

/// Decodes b16encode()'d hexadecimal string into binary.
/// @param in: Hexadecimal string consisting only of 0-9, a-f and A-F.
///            It's the users' duty to ensure characters are within range
/// @return Binary string corresponding to input.
///         Last character chomped if input length is odd.
///         Illegal character intepreted as 0x0.
std::string b16decode(std::string in);

/// Converts non-negative integer string to int.
/// @param in: A string consisting only of 0-9
/// @return Integer whose value equals to input string.
///         Returns -1 when illegal characters occur.
/// @exception It's the user's duty to ensure no overflow will occur.
int string_to_int(std::string in);

/// Print vector of pairs in a Python format.
/// @param out: Targeted output stream, such as std::cout.
/// @param in: Vector of pairs to print.
template <typename _Ta, typename _Tb>
void visualize_dict(std::ostream &out, std::vector<std::pair<_Ta, _Tb>> in) {
    out << "{";
    bool had_items = false;
    for (auto &pr : in) {
        if (had_items)
            out << ", ";
        out << "'" << pr.first << "': '" << pr.second << "'";
        had_items = true;
    }
    out << "}";
}

/// Print std::map in a Python format.
/// @param out: Targeted output stream, such as std::cout.
/// @param in: Map to print.
template <typename _Ta, typename _Tb>
void visualize_dict(std::ostream &out, std::map<_Ta, _Tb> in) {
    std::vector<std::pair<_Ta, _Tb>> vec;
    for (auto &pr : in)
        vec.push_back(pr);
    visualize_dict(out, vec);
}
