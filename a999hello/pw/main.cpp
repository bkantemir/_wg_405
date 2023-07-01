#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdlib.h>

#include "TheApp.h"
#include "platform.h"

static void error_callback(int error, const char* description)
{
    mylog("Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

TheApp theApp;
GLFWwindow* myMainWindow;

int main(void)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    myMainWindow = glfwCreateWindow(640, 480, "OurProject", NULL, NULL);
    if (!myMainWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(myMainWindow, key_callback);

    glfwMakeContextCurrent(myMainWindow);
    gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress); //gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    theApp.run();

    glfwDestroyWindow(myMainWindow);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
