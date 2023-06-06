#pragma once

#include <type_traits>
#include <variant>
#include <concepts>

template<typename F, typename T, typename U>
concept mapper = std::invocable<F, T&&> && std::same_as<std::invoke_result_t<F, T&&>, U>;

/**
 * Generic result container that either contains a value of type `T`, or an error type
 */
template<typename T, typename E>
requires(!std::same_as<T, E>)
struct result {
public:
    using value_type = std::decay_t<T>;
    using error_type = std::decay_t<E>;
    using self = result<T, E>;
    template<typename U>
    using rebound = result<U, error_type>;

    enum class kind { ok, err };
        
    static inline constexpr self ok(value_type&& value) noexcept {
        self me;
        new (&me._val) value_type(value);
        me._kind = kind::ok;
    }
    static inline constexpr self err(error_type&& value) noexcept {
        self me;
        new (&me._err) error_type(value);
        me._kind = kind::err;
    }

    template<typename U, mapper<value_type&&, U> Map>
    inline constexpr rebound<U> map(Map map) && {
        if(_kind == kind::ok) {
            return rebound<U>::ok(map(std::move(_val)));
        } else {
            return rebound<U>::err(_err);
        }
    }

    template<mapper<value_type&&, value_type> AndThen>
    constexpr self and_then(AndThen func) && {
        if(_kind == kind::ok) {
            return self::ok(func(std::move(_val)));
        } else {
            return *this;
        }
    }

    inline constexpr ~result() {
        switch(_kind) {
            case kind::ok: { _val.~value_type(); } break;
            case kind::err: { _err.~error_type(); } break;
        }
    }
private:
    kind _kind;
    union { value_type _val; error_type _err; };
};
