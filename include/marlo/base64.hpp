#pragma once

#include <cstdint>
#include <string>

namespace marlo {
namespace base64 {

/**
 * Encodes `s` into a new base64 string.
 */
std::string encode(std::string_view s);

/**
 * Encodes `data` into a new base64 string.
 */
std::string encode(const std::uint8_t* data, std::size_t size);

/**
 * Encodes `s` into `dst`.
 */
void encode(std::string_view s, std::string& dst);

/**
 * Encodes `data` into `dst`.
 */
void encode(const std::uint8_t* data, std::size_t size, std::string& dst);

/**
 * Decodes `s` into itself, returning whether it succeeded.
 */
bool decode(std::string& s) noexcept;

/**
 * Decodes `s` into `dst`, returning whether it succeeded.
 */
bool decode(std::string_view s, std::uint8_t* dst) noexcept;

/**
 * Whether the given string contains only valid base64-encoded bytes.
 */
bool validate(std::string_view s) noexcept;

inline std::string encode(std::string_view s)
{
    return encode(reinterpret_cast<const std::uint8_t*>(s.data()), s.size());
}

inline std::string encode(const std::uint8_t* data, std::size_t size)
{
    std::string result;
    encode(data, size, result);
    return result;
}

inline void encode(std::string_view s, std::string& dst)
{
    encode(reinterpret_cast<const std::uint8_t*>(s.data()), s.size(), dst);
}

}}
