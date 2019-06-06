//3rd party libraries
#include <GLFW/glfw3.h>

#include <iostream>

//my libraries

//project includes
#include "graphics.hpp"
#include "component/model/model.hpp"

GLFWwindow* initAndMakeWindow() {
	glfwInit();
	auto monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	return glfwCreateWindow(1024, 768, "drone", 0, 0);
}

int main() {
	GLFWwindow* window = initAndMakeWindow();
	component::model::MODEL_STATUS mStatus;
	component::model::Model model = 
		component::model::Model("assets/models/cubetriangles.bin", mStatus);
	if(mStatus != component::model::OK) {
		std::cerr << "Failed to load cube model!" << std::endl;
		exit(EXIT_FAILURE);
	}
	Vulkan vk = Vulkan(window, &model);
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		vk.drawFrame();
	}
	vk.waitIdle();
	glfwDestroyWindow(window);
}
