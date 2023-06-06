#pragma once

#include "result.hpp"
#include <glad/glad.h>
#include <string_view>

/**
 * A compiled vertex or fragment shader with methods to load shaders and set uniforms
 */
class shader {
public:
    enum class err {
        io,
        compile
    };

    /** Load and compile a shader from the specified path */ 
    static result<shader, err> load(std::string_view const path);
private:
    GLint _id;
};
