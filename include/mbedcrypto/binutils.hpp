/** @file binutils.hpp
 *
 * @copyright (C) 2019
 * @date 2019.10.05
 * @author amir zamani <azadkuh@live.com>
 *
 */

#ifndef MBEDCRYPTO_BINUTILS_HPP
#define MBEDCRYPTO_BINUTILS_HPP

#include "mbedcrypto/meta.hxx"
#include <string>

#if defined(QT_CORE_LIB)
#   include <QByteArray>
#endif

//-----------------------------------------------------------------------------
namespace mbedcrypto {
//-----------------------------------------------------------------------------

/// std::string is able to hold both TEXT and Binary data.
/// as encryption is frequently being used with both text strings and binaries,
/// std::string is more convenient than std::vector<unsigned char> or
/// std::basic_string<unsigned char>.
/// although std::vector<unsigned char> is a better options for binary contents.
using buffer_t = std::string;

//-----------------------------------------------------------------------------

/// a view for binary (or text) data.
struct bin_view_t {
    const uint8_t* data = nullptr;
    size_t         size = 0;

    bin_view_t() noexcept = default;

    /// accepts char, unsigned char, ... or any other single-byte type
    template <
        typename T,
        typename Size,
        typename = std::enable_if_t<sizeof(T) == 1 && std::is_integral<Size>::value>
        >
    bin_view_t(const T* buffer, Size length) noexcept
        : data{reinterpret_cast<const uint8_t*>(buffer)},
          size{static_cast<size_t>(length)} {}

    bin_view_t(const char* text_src)
        : data{reinterpret_cast<const uint8_t*>(text_src)},
          size{std::strlen(text_src)} {}

    /// accepts any container with data() and size() member functions.
    /// ex: std::string, std::span, std::array, std::vector, QByteArray, ...
    template <typename Container>
    using is_supported_t = std::enable_if_t<
        std::is_constructible<
            bin_view_t,
            decltype(std::declval<Container>().data()),
            decltype(std::declval<Container>().size())
        >::value
    >;

    template <typename Container, typename = is_supported_t<Container>>
    bin_view_t(const Container& c) noexcept : bin_view_t{c.data(), c.size()} {}

public: // iterators
    using iterator       = const uint8_t*;
    using const_iterator = const uint8_t*;
    iterator       begin()  noexcept       { return data;        }
    iterator       end()    noexcept       { return data + size; }
    const_iterator cbegin() const noexcept { return data;        }
    const_iterator cend()   const noexcept { return data + size; }
}; // struct bin_view_t

//-----------------------------------------------------------------------------

constexpr inline bool
is_empty(const bin_view_t& bv) noexcept {
    return bv.size == 0 || bv.data == nullptr;
}

inline bool
operator==(const std::string& a, bin_view_t b) {
    return b.size == a.size()
        && a.compare(0, b.size, reinterpret_cast<const char*>(b.data), b.size) == 0;
}

inline bool
operator==(bin_view_t a, const std::string& b) {
    return (b == a);
}

//-----------------------------------------------------------------------------
} // namespace mbedcrypto
//-----------------------------------------------------------------------------
#endif // MBEDCRYPTO_BINUTILS_HPP
