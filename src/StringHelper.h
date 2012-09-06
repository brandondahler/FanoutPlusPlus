#ifndef STRINGHELPER_H
#define STRINGHELPER_H

#include <sstream>

namespace StringHelper
{
    template <typename T>
    std::string ToString(const T& value)
    {
        std::ostringstream ss;
        ss << value;
        return ss.str();
    }
};

#endif // STRINGHELPER_H
