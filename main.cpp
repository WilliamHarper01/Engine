#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main()
{
	GLFWwindow* window;
	glfwInit();
	window = glfwCreateWindow(1920, 1080, "bruh", NULL, NULL);
	VkInstance i;
	vkCreateInstance(nullptr, nullptr, &i);
	
	std::cout << "test\n";
	return 0;
}