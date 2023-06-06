#include "log.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main(int argc, char *argv[]) {
    glfwInit();

    auto window = glfwCreateWindow(1280, 720, "Example", NULL, NULL);
    if(window == nullptr) {
        const char* desc;
        glfwGetError(&desc);
        logger::err("Failed to create glfw window: ", desc);

        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        logger::err("Failed to load opengl procedures"); 
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* _, int w, int h) {
        glViewport(0, 0, w, h);
    });

    glClearColor(1.f, 0.f, 1.f, 1.f);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
