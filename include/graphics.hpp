#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <vector>
#include <array>

//3rd party libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

//my libraries
#include "utils/utils.hpp"

class Vulkan : utils::Uncopyable {
	public:
	Vulkan(GLFWwindow*);
	~Vulkan();
	void drawFrame();
	private:
	//static
	const static bool debug;
	const static std::vector<const char*> deviceExtensions;
	const static std::vector<struct Vertex> vertices;
	const static std::vector<uint16_t> indices;
	const static size_t MAX_FRAMES_IN_FLIGHT;

	//members
	std::vector<const char*> extensions;
	std::vector<const char*> validationLayers;
	VkInstance instance;
	GLFWwindow* window;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandPool commandPool;
	//TODO this is going to take a lot of work
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame;

	//VkDebugUtilsMessengerEXT* debugger;
	VkDebugUtilsMessengerEXT debugger;

	//functions
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT*,
		void*
	);
	void setExtensions();
	void setValidationLayers();
	void createInstance();
	void createSurface();
	void createDebugger();
	void createPhysicalDevice();
	bool checkDeviceExtensionSupport(VkPhysicalDevice);
	uint32_t getQueueFamilyIndex(VkPhysicalDevice);
	struct SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice);
	void selectSuitableDevice(const std::vector<VkPhysicalDevice>);
	void createLogicalDeviceAndQueue();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createVertexBuffer();
	std::vector<VkImageView> swapChainImageViews;
	void createCommandPool();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>);
	VkPresentModeKHR chooseSwapPresentMode(
		const std::vector<VkPresentModeKHR>);
	VkExtent2D chooseSwapExtent(
		const VkSurfaceCapabilitiesKHR&);
	VkShaderModule createShaderModule(const std::vector<char>&);
	void createBuffer(
		VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, 
		VkBuffer&, VkDeviceMemory&
	);
	void copyBuffer(VkBuffer, VkBuffer, VkDeviceSize);
	uint32_t findMemoryType(uint32_t, VkMemoryPropertyFlags);
	void createIndexBuffer();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void createCommandBuffers();
	void createSyncObjects();
	void updateUniformBuffer(uint32_t);
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

//yuck
struct Vertex {	
	glm::vec2 pos;
	glm::vec3 color;
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		return attributeDescriptions;
	}
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};


#endif
