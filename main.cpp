#include <iostream>
#include <vector>
#include <glew.h>
#include <GL/gl.h> 
#include <glfw3.h> 

#include "Shader.h"
#include "wrapper.h"

using namespace std;

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main()
{
    if (!glfwInit())
        exit(-1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    int width = 640, height = 600;
    GLFWwindow* window = glfwCreateWindow(width, height, "CENG477 - THE3 - Rabbit Crush", NULL, NULL);

    if (!window)
        exit(-1);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glOrtho(-10, 10, -10, 10, -20, 20);

    ModelData model = load_model("bunny.obj");
    Shader shader("example_diffuse.shader");

    glfwSetKeyCallback(window, keyboard);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        float dist = 2.0f;
        glm::vec3 scale = glm::vec3(std::min(1.0f, 0.3f * dist));
        glm::vec3 color = glm::vec3(0, 1, 0);

        float min_x = -9.0f, max_x = 9.0f;
        float min_y = -9.0f, max_y = 9.0f;
        static float angle = 0.0f;

        for (float x = min_x; x < max_x; x += dist)
        {
            for (float y = min_y; y < max_y; y += dist)
            {
                draw_model(model, shader, { x, y, 0 }, angle, scale, color);
            }
        }

        angle += 0.5f;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}