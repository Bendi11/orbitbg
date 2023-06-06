#pragma once

#include "comptime.hpp"
#include <iostream>
#include <memory>
#include <ostream>
#include <concepts>

namespace logger {

static inline constexpr std::ostream& logfile = std::cout;

template<typename T>
concept Printable = requires(std::ostream& stream, T const&& val) {
    { stream << val } -> std::convertible_to<decltype(stream)>;
};

static_assert(Printable<char*>);


template<Printable P>
static inline constexpr void msg(P val) { logfile << val; }

template<Printable P, Printable... Ps>
static inline constexpr void msg(P val, Ps... ps) {
    if constexpr(comptime::build == comptime::buildkind::debug) {
        msg(val);
        msg(ps...);
    }
}


template<Printable... P>
static inline constexpr void info(P const&&... args) { msg("[INFO] ", std::forward(args)...); }
template<Printable... P>
static inline constexpr void warn(P const&&... args) { msg("[WARN] ", std::forward(args)...); }
template<Printable... P>
static inline constexpr void err(P... args) { msg("[ERR] ", args...); }



}
