#pragma once

#include <string_view>


namespace comptime {
    enum class buildkind {
        debug,
        release,
    };
    
    static inline constexpr const std::string_view build_str = BUILD_TYPE;
    static inline constexpr const buildkind build = (build_str == "Release" || build_str == "MinSizeRel") ? buildkind::release : buildkind::debug;
}
