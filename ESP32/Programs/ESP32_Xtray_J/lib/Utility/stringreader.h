#ifndef STRING_READER_H
#define STRING_READER_H


#if __has_include("Arduino.h")
#define HAS_ARDUINO_H
#endif

#ifdef HAS_ARDUINO_H
#include <Arduino.h>
#endif

#include <cstdlib>
#include <etl/string_view.h>
#include <etl/to_arithmetic.h>
#include <etl/utility.h>
#include <tuple> // etl doesn't support tuples?
#include <type_traits>
#include <utility> // std::make_index_sequence


template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

#define ENABLE_IF(COND) enable_if_t<COND, int> = 0


#ifdef HAS_ARDUINO_H
// Ugly, temporary converter. Ideally, all calls of this functions should be replaced with stringview.
// A disadvantage of string_views is that it doesn't terminate with a null.
inline String toString(etl::string_view sv)
{
    return String(sv.data(), sv.length());
}
#endif

// Copy a stringview to a char buffer.
inline void toBuffer(char* out, const etl::string_view& in) 
{
    strncpy(out, in.data(), in.length());
}

// Check if a stringview equals a C string.
inline bool operator==(const etl::string_view& sv, const char* s)
{
    return strncmp(sv.data(), s, sv.length()) == 0;
}


/**
 * Helper class for buffering strings and handling delimiters.
 *
 * Token-based string consumer with optional back-tracking.
 */
template <template <typename> class R>
class GenericStringReader
{
public:
    struct ContextFrame
    {
        GenericStringReader<R>& reader;
        const size_t prev_index;

        ContextFrame(GenericStringReader<R>& reader) : reader{reader}, prev_index{reader._index} {}

        void fallback()
        {
            reader._index = prev_index;
        }
    };


    GenericStringReader(const char* str) : _text{str} {}
    GenericStringReader(const char* str, size_t len) : _text{str, len} {}
    GenericStringReader(const etl::string_view& sv) : GenericStringReader(sv.data(), sv.length()) {}

#ifdef HAS_ARDUINO_H
    GenericStringReader(const String& str) : _text{str.c_str(), str.length()} {}
#endif

    size_t index() const
    {
        return _index;
    }
    const etl::string_view& str() const
    {
        return _text;
    }


    /**
     * @brief   Check if there are bytes available to read.
     */
    operator bool() const
    {
        return available() > 0;
    }

    /**
     * @brief   Get the available number of bytes that can be read.
     */
    size_t available() const
    {
        return _text.length() - _index;
    }

    /**
     * @brief   Reset the reader's cursor to start from the beginning of the string.
     */
    void reset()
    {
        _index = 0;
    }

    /**
     * @brief   Set the default delimiter to be used in the read functions. If no delimiter
     *          is provided in those parameters, this delimiter is used.
     */
    GenericStringReader& delim(char c)
    {
        _delim = c;
        return *this;
    }

    /**
     * @brief   Reads a string until the given delimiter is seen (or until the end of the string).
     * @param   delim The delimiter to read until. If no delimiter is encountered, reads until the end of the string.
     */
    etl::string_view readUntil(char delim)
    {
        if (!available())
            return "";

        delim = delim ? delim : _delim;

        size_t pos = _index;
        _index = _text.find(delim, pos);

        if (_index == etl::string_view::npos) {
            // Delimiter not found.
            _index = _text.length();
        }
        return _text.substr(pos, _index - pos);
    }

    etl::string_view readToken(char delim)
    {
        auto sv = readUntil(delim);
        skip(delim);
        return sv;
    }

    etl::string_view readLine()
    {
        return readToken('\n');
    }

    /**
     * @brief   Discards characters while equal to `c`.
     */
    void dropWhile(char c)
    {
        for (; _index < _text.length() && _text[_index] == c; _index++)
            ;
    }

    /**
     * @brief   Discards (at most one) character if equal to `c`.
     */
    bool skip(char c)
    {
        if (_text[_index] == c) {
            _index++;
            return true;
        }
        return false;
    }

    etl::string_view rest()
    {
        if (!available())
            return "";

        auto s = _text.substr(_index);
        _index = _text.length();
        return s;
    }

    template <typename T>
    etl::optional<T> readSafe(char delim = 0)
    {
        return R<T>{*this}.readSafe(delim ? delim : _delim);
    }

    template <typename T>
    T read(char delim = 0)
    {
        return R<T>{*this}.read(delim ? delim : _delim);
    }

    template <typename... Ts, ENABLE_IF(sizeof...(Ts) >= 2)>
    std::tuple<Ts...> read(const char* between = "<>", char delim = ',')
    {
        return R<std::tuple<Ts...>>{*this, between}.read(delim);
    }

    template <typename... Ts, ENABLE_IF(sizeof...(Ts) >= 2)>
    etl::optional<std::tuple<Ts...>> readSafe(const char* between = "<>", char delim = ',')
    {
        return R<std::tuple<Ts...>>{*this, between}.readSafe(delim);
    }

private:
    etl::string_view _text;
    size_t _index = 0;
    char _delim = ' ';
};


template <typename T, typename Enable = void>
struct Parser;

template <typename T>
using DefaultParser = Parser<T, void>;

using StringReader = GenericStringReader<DefaultParser>;


template <typename T>
struct Parser<T, enable_if_t<(std::is_arithmetic<T>::value)>>
{
    StringReader& reader;

    T read(char delim)
    {
        auto s = reader.readUntil(delim);
        return etl::to_arithmetic<T>(s.data(), s.length());
    }

    etl::optional<T> readSafe(char delim)
    {
        StringReader::ContextFrame ctx{reader};

        auto s = reader.readUntil(delim);
        auto res = etl::to_arithmetic<T>(s.data(), s.length());
        if (!res) {
            // Failed to convert.
            ctx.fallback();
            return etl::nullopt;
        }
        return etl::optional<T>{res.value()};
    }
};


template <>
struct Parser<etl::string_view>
{
    StringReader& reader;

    etl::string_view read(char delim)
    {
        return reader.readUntil(delim);
    }

    etl::optional<etl::string_view> readSafe(char delim)
    {
        return etl::optional<etl::string_view>{reader.readUntil(delim)};
    }
};


#ifdef HAS_ARDUINO_H
template <>
struct Parser<String>
{
    StringReader& reader;

    String read(char delim)
    {
        auto s = reader.read<etl::string_view>(delim);
        return String(s.data(), s.length());
    }

    etl::optional<String> readSafe(char delim)
    {
        auto s = reader.read<etl::string_view>(delim);
        return etl::optional<String>{String(s.data(), s.length())};
    }
};
#endif


/**
 * @brief   Parse a 1-tuple, unpack and parse regularly.
 */
template <typename T>
struct Parser<std::tuple<T>> : Parser<T>
{
};


#if __cplusplus >= 201700L
/**
 * @brief   Parse a tuple in the format <v0, v1, ..., vn>.
 *
 *          The C++17 Parser works with tuples of any size.
 */
template <typename... Ts>
struct Parser<std::tuple<Ts...>, enable_if_t<(sizeof...(Ts) >= 2)>>
{
    StringReader& reader;
    char open = '<';
    char close = '>';

    Parser(StringReader& reader, char open, char close) : reader{reader}, open{open}, close{close} {}
    Parser(StringReader& reader, const char* between) : Parser(reader, between[0], between[1]) {}

    std::tuple<Ts...> read(char delim)
    {
        reader.skip(open);
        std::tuple<Ts...> tup;
        read_inner<false>(delim, tup);
        reader.skip(close);
        return tup;
    }

    etl::optional<std::tuple<Ts...>> readSafe(char delim)
    {
        StringReader::ContextFrame ctx{reader};

        if (!reader.skip(open)) {
            return etl::nullopt;
        }

        std::tuple<Ts...> tup;
        if (!read_inner<true>(delim, tup)) {
            ctx.fallback();
            return etl::nullopt;
        }

        if (!reader.skip(close)) {
            ctx.fallback();
            return etl::nullopt;
        }

        return tup;
    }

private:
    template <bool Safe>
    bool read_inner(char delim, std::tuple<Ts...>& tup)
    {
        // Pass on the reading to an impl function, which has compile-time indices.
        return read_inner_impl<Safe>(delim, tup, std::make_index_sequence<sizeof...(Ts)>{});
    }

    template <bool Safe, size_t... Idxs>
    bool read_inner_impl(char delim, std::tuple<Ts...>& tup, std::index_sequence<Idxs...>)
    {
        // Use a fold-expr with `&&`, to trigger short-circuit logic. If a read fails, then subsequent reads are
        // cancelled.
        return (read_one<Safe, Idxs>(delim, tup) && ...);
    }

    template <bool Safe, size_t Idx>
    bool read_one(char delim, std::tuple<Ts...>& tup)
    {
        using T = std::decay_t<decltype(std::get<Idx>(tup))>;
        constexpr bool end = Idx + 1 == sizeof...(Ts); // True if we're reading the last element.

        delim = end ? close : delim;

        if constexpr (Safe) {
            auto val = reader.readSafe<T>(delim);
            if (!val)
                return false;

            std::get<Idx>(tup) = *val;
            // Skip delim if not at end.
            // end ? true : reader.skip(delim);
            return end || reader.skip(delim);

        } else {
            std::get<Idx>(tup) = reader.read<T>(delim);
            if constexpr (!end) {
                reader.skip(delim);
            }
            return true;
        }
    }
};

#elif __cplusplus >= 201100L
/**
 * @brief   Parse a tuple in the format <v0, v1, ..., vn>.
 *
 *          The C++11 implementation generates classes using macros and defines Parsers up to a 5-tuple.
 */

#define READ_AT(idx, last)                                                    \
    do {                                                                      \
        std::get<idx>(tup) = reader.read<type_at<idx>>(last ? close : delim); \
        reader.skip(last ? close : delim);                                    \
    } while (0)

#define READ_SAFE_AT(idx, last)                                         \
    do {                                                                \
        auto val = reader.readSafe<type_at<idx>>(last ? close : delim); \
        if (!val) {                                                     \
            ctx.fallback();                                             \
            return etl::nullopt;                                        \
        }                                                               \
        if (!reader.skip(last ? close : delim)) {                       \
            ctx.fallback();                                             \
            return etl::nullopt;                                        \
        }                                                               \
        std::get<idx>(tup) = *val;                                      \
    } while (0);

#define X_TYPENAME_EXPAND(A) , typename A
#define X_EXPAND(A)          , A

#define DEFINE_TUPLE_PARSER(TYPES, BODY, BODY_SAFE)                                                          \
    template <typename T TYPES(X_TYPENAME_EXPAND)>                                                           \
    struct Parser<std::tuple<T TYPES(X_EXPAND)>>                                                             \
    {                                                                                                        \
        StringReader& reader;                                                                                \
        char open = '<';                                                                                     \
        char close = '>';                                                                                    \
                                                                                                             \
        Parser(StringReader& reader, char open, char close) : reader{reader}, open{open}, close{close} {}    \
        Parser(StringReader& reader, const char* between) : Parser(reader, between[0], between[1]) {}        \
                                                                                                             \
        template <size_t Idx>                                                                                \
        using type_at = typename std::decay<decltype(std::get<Idx>(std::tuple<T TYPES(X_EXPAND)>{}))>::type; \
                                                                                                             \
                                                                                                             \
        std::tuple<T TYPES(X_EXPAND)> read(char delim)                                                       \
        {                                                                                                    \
            reader.skip(open);                                                                               \
            std::tuple<T TYPES(X_EXPAND)> tup;                                                               \
            BODY;                                                                                            \
            return tup;                                                                                      \
        }                                                                                                    \
                                                                                                             \
        etl::optional<std::tuple<T TYPES(X_EXPAND)>> readSafe(char delim)                                    \
        {                                                                                                    \
            StringReader::ContextFrame ctx{reader};                                                          \
                                                                                                             \
            if (!reader.skip(open)) {                                                                        \
                return etl::nullopt;                                                                         \
            }                                                                                                \
                                                                                                             \
            std::tuple<T TYPES(X_EXPAND)> tup;                                                               \
            BODY_SAFE;                                                                                       \
            return etl::optional<std::tuple<T TYPES(X_EXPAND)>>{tup};                                        \
        }                                                                                                    \
    };

#define TYPES(X) X(U)
DEFINE_TUPLE_PARSER(
    TYPES,
    {
        READ_AT(0, false);
        READ_AT(1, true);
    },
    {
        READ_SAFE_AT(0, false);
        READ_SAFE_AT(1, true);
    })

#undef TYPES
#define TYPES(X) X(U) X(V)
DEFINE_TUPLE_PARSER(
    TYPES,
    {
        READ_AT(0, false);
        READ_AT(1, false);
        READ_AT(2, true);
    },
    {
        READ_SAFE_AT(0, false);
        READ_SAFE_AT(1, false);
        READ_SAFE_AT(2, true);
    })

#undef TYPES
#define TYPES(X) X(U) X(V) X(W)
DEFINE_TUPLE_PARSER(
    TYPES,
    {
        READ_AT(0, false);
        READ_AT(1, false);
        READ_AT(2, false);
        READ_AT(3, true);
    },
    {
        READ_SAFE_AT(0, false);
        READ_SAFE_AT(1, false);
        READ_SAFE_AT(2, false);
        READ_SAFE_AT(3, true);
    })

#undef TYPES
#define TYPES(X) X(U) X(V) X(W) X(Y)
DEFINE_TUPLE_PARSER(
    TYPES,
    {
        READ_AT(0, false);
        READ_AT(1, false);
        READ_AT(2, false);
        READ_AT(3, false);
        READ_AT(4, true);
    },
    {
        READ_SAFE_AT(0, false);
        READ_SAFE_AT(1, false);
        READ_SAFE_AT(2, false);
        READ_SAFE_AT(3, false);
        READ_SAFE_AT(4, true);
    })

#undef READ_AT
#undef READ_SAFE_AT
#undef X_EXPAND
#undef X_TYPENAME_EXPAND
#undef TYPES


#endif

#undef ENABLE_IF


#endif