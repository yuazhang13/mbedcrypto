/** @file hash.hpp
 *
 * @copyright (C) 2016
 * @date 2016.03.05
 * @author amir zamani <azadkuh@live.com>
 *
 * related cmake build options:
 *  - MBEDCRYPTO_MD2
 *  - MBEDCRYPTO_MD4
 *  - MBEDCRYPTO_RIPEMD160
 *
 */

#ifndef MBEDCRYPTO_HASH_HPP
#define MBEDCRYPTO_HASH_HPP

#include "mbedcrypto/types.hpp"
#include "mbedcrypto/binutils.hpp"
#include "mbedcrypto/errors.hpp"

//-----------------------------------------------------------------------------
namespace mbedcrypto {
//-----------------------------------------------------------------------------

/// returns the length of a hash algorithm in byte or zero as error.
size_t hash_size(hash_t) noexcept;

/// makes the hash value for a buffer in single operation.
/// the output and output_size should be large enough @sa hash_size()
std::error_code
make_hash(
    bin_view_t input,
    hash_t     algorithm,
    uint8_t*   output,
    size_t&    output_size) noexcept;

/// makes the hash value for a file.
/// the output and output_size should be large enough @sa hash_size()
std::error_code
make_file_hash(
    const char* filename,
    hash_t      algorithm,
    uint8_t*    output,
    size_t&     output_size) noexcept;

//-----------------------------------------------------------------------------

/** a reusable hash utility.
 *
 * @code
 * hash h;
 *
 * h.start(hash_t::sha256);
 * for (...) {
 *     ...
 *     h.update(chunk);
 *     ...
 * }
 * std::vector<uint8_t> digest;
 * h.finish(digest);
 *
 * h.start(hash_t::sha512);
 * h.update(...);
 * h.finish(digest);
 *
 * @endcode
 */
struct hash
{
    /// resets and starts digesting by specified algorithm.
    std::error_code start(hash_t) noexcept;
    /** feeds a chunk of data into an ongoing message-digest computation.
     * call start() before calling this function. you may call this function
     * multiple times.  afterwards, call finish().
     */
    std::error_code update(bin_view_t chunk) noexcept;
    /** finishes the digest operation and writes out into output.
     * if output == nullptr or output_size == 0, returns the required hash size
     * into output_size.
     */
    std::error_code finish(uint8_t* output, size_t& output_size) noexcept;

    template <class Container>
    std::error_code finish(Container& output) {
        size_t osize = 0;
        finish(nullptr, osize);
        output.resize(osize);
        return finish(reinterpret_cast<uint8_t*>(&output[0]), osize);
    }

    // move only
    hash();
    ~hash();

    hash(hash&&) noexcept = default;
    hash& operator=(hash&&) noexcept = default;

protected:
    struct impl;
    std::unique_ptr<impl> pimpl;
}; // class hash

//-----------------------------------------------------------------------------
// helper api

template <class Container>
inline std::error_code
make_hash(Container& digest, bin_view_t input, hash_t algo) {
    auto size = hash_size(algo);
    digest.resize(size);
    return make_hash(input, algo, reinterpret_cast<uint8_t*>(&digest[0]), size);
}

template <class Container>
inline std::pair<Container, std::error_code>
make_hash(bin_view_t input, hash_t algo) {
    Container digest;
    auto      ec = make_hash(digest, input, algo);
    return {digest, ec};
}

template <class Container>
inline auto
make_sha1(bin_view_t input) {
    return make_hash<Container>(input, hash_t::sha1);
}

template <class Container>
inline auto
make_sha256(bin_view_t input) {
    return make_hash<Container>(input, hash_t::sha256);
}

template <class Container>
inline auto
make_sha512(bin_view_t input) {
    return make_hash<Container>(input, hash_t::sha512);
}


//-----------------------------------------------------------------------------
} // namespace mbedcrypto
//-----------------------------------------------------------------------------
#endif // MBEDCRYPTO_HASH_HPP
