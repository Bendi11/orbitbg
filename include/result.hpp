#pragma once

#include <type_traits>
#include <variant>
#include <concepts>

template<typename F, typename T, typename U>
concept mapper = std::invocable<F, T&&> && std::same_as<std::invoke_result_t<F, T&&>, U>;

enum class unresolved {};

/**
 * Generic result container that either contains a value of type `T`, or an error type
 */
template<typename T = unresolved, typename E = unresolved>
requires(!std::same_as<T, E>)
struct result {
public:
    using value_type = std::decay_t<T>;
    using error_type = std::decay_t<E>;
    using self = result<T, E>;
    template<typename U>
    using rebound = result<U, error_type>;

    enum class kind { ok, err };

    inline constexpr result(value_type&& val) : _kind(kind::ok), _val(std::move(val)) {}
    inline constexpr result(error_type&& err) : _kind(kind::err), _err(std::move(err)) {}
    
    inline constexpr result(result<T, unresolved>&& other) requires(!std::same_as<T, unresolved>) : _kind(kind::ok), _val(std::move(other._val)) {}
    inline constexpr result(result<unresolved, E>&& other) requires(!std::same_as<E, unresolved>) : _kind(kind::err), _err(std::move(other._err)) {}

    template<typename U, mapper<value_type&&, U> Map>
    inline constexpr rebound<U> map(Map map) && {
        if(_kind == kind::ok) {
            return rebound<U>(map(std::move(_val)));
        } else {
            return rebound<U>(std::move(_err));
        }
    }

    template<mapper<value_type&&, value_type> AndThen>
    constexpr self and_then(AndThen func) && {
        if(_kind == kind::ok) {
            return self(func(std::move(_val)));
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
    result() {}

    kind _kind;
    union { value_type _val; error_type _err; };

    template<typename O, typename O2>
    requires(!std::same_as<O, O2>)
    friend struct result;
};
