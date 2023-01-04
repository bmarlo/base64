#include "marlo/base64.hpp"
#include <array>

namespace marlo {
namespace base64 {

constexpr std::array<char, 64> alphabet = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/',
};

void encode(const std::uint8_t* data, std::size_t size, std::string& dst)
{
    std::size_t i = 0;
    auto k = dst.size();
    auto pads = 3 - size % 3;
    pads *= pads < 3;
    dst.resize(k + (size + pads) * 4 / 3);
    while (!(size - i < 3)) {
        dst[k++] = alphabet[data[i] >> 2];
        dst[k++] = alphabet[((data[i] & 0x03) << 4) | (data[i + 1] >> 4)];
        dst[k++] = alphabet[((data[i + 1] & 0x0f) << 2) | (data[i + 2]) >> 6];
        dst[k++] = alphabet[data[i + 2] & 0x3f];
        i += 3;
    }

    switch (pads) {
    case 1:
        dst[k++] = alphabet[data[i] >> 2];
        dst[k++] = alphabet[((data[i] & 0x03) << 4) | (data[i + 1] >> 4)];
        dst[k++] = alphabet[(data[i + 1] & 0x0f) << 2];
        dst[k++] = '=';
        break;
    case 2:
        dst[k++] = alphabet[data[i] >> 2];
        dst[k++] = alphabet[(data[i] & 0x03) << 4];
        dst[k++] = '=';
        dst[k++] = '=';
        break;
    default:
        break;
    }
}

constexpr std::array<std::int8_t, 256> revbet = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

template<typename cb_t>
bool decode_impl(std::string_view s, cb_t on_decoded)
{
    if (s.empty()) {
        return true;
    }

    if (s.size() % 4) {
        return false;
    }

    struct {
        std::uint8_t a, b, c, d;
    } ctx;

    auto get_data = [&](std::size_t i) {
        int error = 0;
        std::uint8_t badval = -1;
        error |= ((ctx.a = revbet[static_cast<std::uint8_t>(s[i])]) == badval) << 3;
        error |= ((ctx.b = revbet[static_cast<std::uint8_t>(s[i + 1])]) == badval) << 2;
        error |= ((ctx.c = revbet[static_cast<std::uint8_t>(s[i + 2])]) == badval) << 1;
        error |= ((ctx.d = revbet[static_cast<std::uint8_t>(s[i + 3])]) == badval);
        return error;
    };

    std::size_t i = 0;
    while (i + 4 < s.size()) {
        if (get_data(i)) {
            return false;
        }

        on_decoded((ctx.a << 2) | (ctx.b >> 4));
        on_decoded((ctx.b << 4) | (ctx.c >> 2));
        on_decoded((ctx.c << 6) | ctx.d);
        i += 4;
    }

    std::uint8_t pads = 0;
    auto error = get_data(i);
    if (s[i + 2] == '=') {
        if (s[i + 3] != '=') {
            return false;
        }
        pads = 2;
    } else {
        if (s[i + 3] == '=') {
            pads = 1;
        }
    }

    switch (pads) {
    case 1:
        error &= ~0x01;
        on_decoded((ctx.a << 2) | (ctx.b >> 4));
        on_decoded((ctx.b << 4) | (ctx.c >> 2));
        break;
    case 2:
        error &= ~0x03;
        on_decoded((ctx.a << 2) | (ctx.b >> 4));
        break;
    default:
        on_decoded((ctx.a << 2) | (ctx.b >> 4));
        on_decoded((ctx.b << 4) | (ctx.c >> 2));
        on_decoded((ctx.c << 6) | ctx.d);
        break;
    }

    return !error;
}

bool decode(std::string& s) noexcept
{
    std::size_t i = 0;
    auto push = [&](int val) {
        s[i++] = static_cast<char>(val);
    };

    if (decode_impl(s, std::move(push))) {
        s.resize(i);
        return true;
    }
    return false;
}

bool decode(std::string_view s, std::uint8_t* dst) noexcept
{
    auto push = [&](int val) {
        *dst++ = static_cast<std::uint8_t>(val);
    };
    return decode_impl(s, std::move(push));
}

bool validate(std::string_view s) noexcept
{
    return decode_impl(s, [](int) {});
}

}}
