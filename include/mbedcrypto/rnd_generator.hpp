/** @file rnd_generator.hpp
 *
 * @copyright (C) 2016
 * @date 2016.03.07
 * @version 1.0.0
 * @author amir zamani <azadkuh@live.com>
 *
 */

#ifndef MBEDCRYPTO_RND_GENERATOR_HPP
#define MBEDCRYPTO_RND_GENERATOR_HPP

#include "types.hpp"
///////////////////////////////////////////////////////////////////////////////
namespace mbedcrypto {
///////////////////////////////////////////////////////////////////////////////

/// counter mode deterministic random byte generator (CTR_DRBG).
/// @note mbedtls (mbedcrypto) CTR_DRBG based on AES-256 (NIST SP 800-90) and
///  internally uses an entropy collection modules.
class rnd_generator
{
public:
    /// initializes both entropy collector and CTR_DRBG
    rnd_generator();

    /// optional custom data can be provided in addition to the more generic entropy source.
    /// useful when using random objects (possibly on different threads).
    ///  each thread can have a unique custom byte for better security.
    /// This makes sure that the random generators between the different
    ///  threads have the least amount of correlation possible and can
    ///  thus be considered as independent as possible.
    explicit rnd_generator(const buffer_t& custom);

    ~rnd_generator();

    /// returns a random binary buffer with specified length
    /// @note automatically reseeds if reseed_interval is passed.
    /// length can be in any size because underlying class makes random in chunks.
    auto make(size_t length) -> buffer_t;
    /// low level overload
    int  make(unsigned char* buffer, size_t length)noexcept;

public: // auxiliary methods
    /// set entropy read length. default: 32/48 (sha256/sha512).
    /// based on build configs, uses sha256 or sha512
    void entropy_length(size_t) noexcept;
    /// set reseeding interval. default: 10000 calls
    void reseed_interval(size_t) noexcept;
    /// if set to true, entropy is used with each call! quite expensive but more secure.
    /// default: false
    void prediction_resistance(bool) noexcept;

    /// reseeds (extract data from entropy)
    void reseed();
    /// overload with custom data
    void reseed(const buffer_t& custom);
    /// low level overload, nullptr, 0 are valid
    int  reseed(const unsigned char* custom, size_t length) noexcept;

    /// updates CTR_DRBG internal state with additional (custom) data
    void update(const buffer_t& additional);
    /// low level overload
    void update(const unsigned char* additional, size_t length) noexcept;

    // move only
    rnd_generator(const rnd_generator&)            = delete;
    rnd_generator(rnd_generator&&)                 = default;
    rnd_generator& operator=(const rnd_generator&) = delete;
    rnd_generator& operator=(rnd_generator&&)      = default;

protected:
    struct impl;
    std::unique_ptr<impl> pimpl;
}; // rnd_generator

///////////////////////////////////////////////////////////////////////////////
} // namespace mbedcrypto
///////////////////////////////////////////////////////////////////////////////
#endif // MBEDCRYPTO_RND_GENERATOR_HPP
