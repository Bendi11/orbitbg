#include "shader.hpp"
#include "log.hpp"
#include "result.hpp"
#include <cstdio>
#include <fstream>
#include <string_view>

result<shader, shader::err> shader::load(char const* path) noexcept {
    std::FILE *infile = std::fopen(path, "r");
    if(infile == nullptr) {
        return shader::err::io;
    }

    std::string txt{};
    std::fseek(infile, 0, SEEK_END);
    auto len = std::ftell(infile);
    txt.reserve(len);
    std::rewind(infile);
    std::fread(txt.data(), sizeof(char), txt.size(), infile);
    std::fclose(infile);

    return compile(txt.c_str());
}

result<shader, shader::err> shader::compile(char const* txtBuf) noexcept {
    GLint id;
    id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(id, 1, &txtBuf, nullptr);

    glCompileShader(id);
    int compileStatus;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE) {
        int logLen;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);

        std::string compileLog{};
        compileLog.resize(logLen, '\0');
        glGetShaderInfoLog(id, logLen, &logLen, compileLog.data());
        logger::err("Failed to compile shader: ", compileLog);
        return shader::err::compile;
    }

    return shader(id);
}
