#pragma once

#include "result.hpp"
#include <glad/glad.h>
#include <string_view>

/**
 * A compiled vertex or fragment shader with methods to load shaders and set uniforms
 */
class shader {
public:
    shader(shader const&) = delete;
    shader& operator=(shader const&) = delete;

    shader(shader&&) = default;
    shader& operator=(shader&&) = default;
    
    enum class err {
        io,
        compile
    };

    /** Load and compile a shader from the specified path */ 
    static result<shader, err> load(char const*  path) noexcept;
    /** Compile a shader from the specified string */
    static result<shader, err> compile(char const* str) noexcept;

    inline ~shader() {
        glDeleteShader(_id);
    }

private:
    inline constexpr shader(GLint id) : _id(id) {}
    friend class shader_program;

    inline constexpr GLint id() const noexcept { return _id; }

    GLint _id;
};

class shader_program {
public:
    shader_program(shader&& vertex, shader&& fragment);

private:
    GLint _id;
};
