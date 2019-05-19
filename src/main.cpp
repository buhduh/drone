//3rd party libraries
#include <GLFW/glfw3.h>

#include <iostream>

//my libraries

//project includes
#include "graphics.hpp"

GLFWwindow* initAndMakeWindow() {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(1024, 768, "drone", 0, 0);
}

int main() {
	GLFWwindow* window = initAndMakeWindow();
	Vulkan vk = Vulkan(window);
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		vk.drawFrame();
	}
	glfwDestroyWindow(window);
}
