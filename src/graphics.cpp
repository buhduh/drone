//"ephemeral", probably delete
#include <iostream>

#include <stdexcept>
#include <set>
#include <string>
#include <chrono>

//3rd party libraries

//my libraries

//project includes
#include "graphics.hpp"

#ifdef DEBUG
const bool Vulkan::debug = true;
#else
const bool Vulkan::debug = false;
#endif

const std::vector<const char*> Vulkan::deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const size_t Vulkan::MAX_FRAMES_IN_FLIGHT(2);

//I don't think initialization lists are good for non constructor args
Vulkan::Vulkan(GLFWwindow* window, component::model::Model* model = nullptr) 
	: extensions(0) 
	, frameBufferResized(false)
	, validationLayers(0)
	, instance(VK_NULL_HANDLE)
	, window(window)
	, surface(VK_NULL_HANDLE)
	, physicalDevice(VK_NULL_HANDLE)
	, device(VK_NULL_HANDLE)
	, graphicsQueue(VK_NULL_HANDLE)
	, presentQueue(VK_NULL_HANDLE)
	, swapChain(VK_NULL_HANDLE)
	, swapChainImages(0)
	, swapChainImageFormat(VK_FORMAT_UNDEFINED)
	, swapChainExtent({0,0})
	, swapChainImageViews(0)
	, descriptorSetLayout(VK_NULL_HANDLE)
	, graphicsPipeline(VK_NULL_HANDLE)
	, pipelineLayout(VK_NULL_HANDLE)
	, swapChainFramebuffers(0)
	, commandPool(VK_NULL_HANDLE)
	, vertexBuffer(VK_NULL_HANDLE)
	, vertexBufferMemory(VK_NULL_HANDLE)
	, indexBuffer(VK_NULL_HANDLE)
	, indexBufferMemory(VK_NULL_HANDLE)
	, uniformBuffers(0)
	, uniformBuffersMemory(0)
	, descriptorPool(0)
	, descriptorSets(0)
	, commandBuffers(0)
	, imageAvailableSemaphores(0)
	, renderFinishedSemaphores(0)
	, inFlightFences(0)
	, currentFrame(0)
	, model(model)
{
	createInstance();
	createSurface();
	createDebugger();
	createPhysicalDevice();
	createLogicalDeviceAndQueue();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createSyncObjects();
}

Vulkan::~Vulkan() {
	cleanupSwapChain();
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);
	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}
	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void Vulkan::waitIdle() {
	vkDeviceWaitIdle(device);
}

void Vulkan::cleanupSwapChain() {
	for(auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
	vkFreeCommandBuffers(
		device, commandPool, 
		static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data()
	);
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);
	for(auto i : swapChainImageViews) {
		vkDestroyImageView(device, i, nullptr);
	}
	vkDestroySwapchainKHR(device, swapChain, nullptr);
	for(size_t i = 0; i < swapChainImages.size(); ++i) {
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

void Vulkan::recreateSwapChain() {
	vkDeviceWaitIdle(device);
	cleanupSwapChain();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createUniformBuffers();
	createDescriptorPool();
	createCommandBuffers();
}

void Vulkan::updateUniformBuffer(uint32_t currentImage) {
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = 
		std::chrono::duration<float, std::chrono::seconds::period>(
			currentTime - startTime
		).count();
	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(
		glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)
	);
	ubo.view = glm::lookAt(
		glm::vec3(2.0f, 2.0f, 2.0f), 
		glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3(0.0f, 0.0f, 1.0f)
	);
	ubo.proj = glm::perspective(
		glm::radians(45.0f), 
		swapChainExtent.width / (float) swapChainExtent.height, 
		0.1f, 10.0f
	);
	ubo.proj[1][1] *= -1;
	void* data;
	vkMapMemory(
		device, uniformBuffersMemory[currentImage], 0, 
		sizeof(ubo), 0, &data
	);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
}

void Vulkan::drawFrame() {
	vkWaitForFences(
		device, 1, &inFlightFences[currentFrame], 
		VK_TRUE, std::numeric_limits<uint64_t>::max()
	);
    uint32_t imageIndex;
    auto res = vkAcquireNextImageKHR(
		device, swapChain, std::numeric_limits<uint64_t>::max(), 
		imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex
	);
	//TODO This is ALWAYS suboptimal, i should probably fix this
	//TODO need to handle resizing
	if(res == VK_ERROR_OUT_OF_DATE_KHR) {
		//TODO should I print something?
		recreateSwapChain();
		return;
	}
	//TODO fix this???
	//if(res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
	if(res != VK_SUCCESS) {
		throw std::runtime_error("Failed acquiring next swap chain image.");
	}

	updateUniformBuffer(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	vkResetFences(device, 1, &inFlightFences[currentFrame]);
	if(
		vkQueueSubmit(
			graphicsQueue, 1, &submitInfo, 
			inFlightFences[currentFrame]) != VK_SUCCESS
		) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = {swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	res = vkQueuePresentKHR(presentQueue, &presentInfo);
	if(res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || frameBufferResized) {
		frameBufferResized = false;
		recreateSwapChain();
	} else if(res != VK_SUCCESS) { 
		throw std::runtime_error("Failed to present swap chain.");
	}
	vkQueueWaitIdle(presentQueue);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Vulkan::createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		auto iRes = vkCreateSemaphore(
			device, &semaphoreInfo, nullptr, 
			&imageAvailableSemaphores[i]
		);
		auto rRes = vkCreateSemaphore(
			device, &semaphoreInfo, nullptr, 
			&renderFinishedSemaphores[i]
		);
		auto fRes = vkCreateFence(
			device, &fenceInfo, nullptr, 
			&inFlightFences[i]
		);
		if(iRes || rRes || fRes) {
			throw std::runtime_error("failed to create semaphores!");
		}
	}
}

void Vulkan::createCommandBuffers() {
	if(!swapChainFramebuffers.size()) {
		throw std::runtime_error("swapChainFramebuffers.size() can't be 0.");
	}
	commandBuffers.resize(swapChainFramebuffers.size());
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
	if(
		vkAllocateCommandBuffers(
			device, &allocInfo, 
			commandBuffers.data()) != VK_SUCCESS
		) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
	for(size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional
		if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error(
				"failed to begin recording command buffer!"
			);
		}
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChainExtent;
		VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(
			commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE
		);
		vkCmdBindPipeline(
			commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline
		);
		VkBuffer vertexBuffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(
			commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16
		);
		vkCmdBindDescriptorSets(
			commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, 
			pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr
		);
		vkCmdDrawIndexed(
			commandBuffers[i], static_cast<uint32_t>(model->indices.size()), 
			1, 0, 0, 0
		);
		vkCmdEndRenderPass(commandBuffers[i]);
		if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void Vulkan::createDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();
	descriptorSets.resize(swapChainImages.size());
	if(
		vkAllocateDescriptorSets(
			device, &allocInfo, descriptorSets.data()
		) != VK_SUCCESS
	) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	for(size_t i = 0; i < swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);
		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional
		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}
}

void Vulkan::createDescriptorPool() {
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(swapChainImages.size());
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());;
	poolInfo.flags = 0;
	if(
		vkCreateDescriptorPool(
			device, &poolInfo, nullptr, 
			&descriptorPool) != VK_SUCCESS
		) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void Vulkan::createUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());
	for(size_t i = 0; i < swapChainImages.size(); ++i) {
		createBuffer(
			bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			uniformBuffers[i], uniformBuffersMemory[i]
		);
	}
}

void Vulkan::createIndexBuffer() {
    VkDeviceSize bufferSize = 
		sizeof(component::model::index_size_t) 
		* model->indices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(
		bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, stagingBufferMemory
	);
    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, model->indices.data(), (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);
    createBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory
	);
    copyBuffer(stagingBuffer, indexBuffer, bufferSize);
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Vulkan::createBuffer(
	VkDeviceSize size, VkBufferUsageFlags usage, 
	VkMemoryPropertyFlags properties, VkBuffer& buffer, 
	VkDeviceMemory& bufferMemory
) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if(
		vkCreateBuffer(
			device, &bufferInfo, nullptr, 
			&buffer) != VK_SUCCESS
		) {
        throw std::runtime_error("failed to create vertex buffer!");
    }
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(
		memRequirements.memoryTypeBits, properties
	);
	if(
		vkAllocateMemory(
			device, &allocInfo, nullptr, 
			&bufferMemory) != VK_SUCCESS
		) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}
	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

uint32_t Vulkan::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if(
			(typeFilter & (1 << i)) && 
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties
			) {
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

void Vulkan::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	vkEndCommandBuffer(commandBuffer);
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

//TODO this is gonna take a lot of work!!!!
void Vulkan::createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(component::model::vertex) * model->vertices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(
		bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, stagingBufferMemory
	);
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, model->vertices.data(), (size_t) bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);
	createBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		vertexBuffer, vertexBufferMemory
	);
	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Vulkan::createCommandPool() {
	uint32_t qFamilyIndex = getQueueFamilyIndex(physicalDevice);
	if(qFamilyIndex == UINT32_MAX) {
		throw std::runtime_error("Error retreiving queue family index!");
	}
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = qFamilyIndex;
	poolInfo.flags = 0; // Optional
	if(
		vkCreateCommandPool(
			device, &poolInfo, 
			nullptr, &commandPool) != VK_SUCCESS
		) {
		throw std::runtime_error("Failed to create command pool!");
	}
}

void Vulkan::createFramebuffers() {
	if(!swapChainImageViews.size()) {
		throw std::runtime_error("swapChainImageViews.size() can't be 0.");
	}
	swapChainFramebuffers.resize(swapChainImageViews.size());
	for(size_t i = 0; i < swapChainImageViews.size(); ++i) {
		VkImageView attachments[] = {
			swapChainImageViews[i]
		};
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;
    	if(
			vkCreateFramebuffer(
				device, &framebufferInfo, nullptr, 
				&swapChainFramebuffers[i]) != VK_SUCCESS
			) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

VkShaderModule Vulkan::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule shaderModule;
	if(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Could not create shader module!");		
	}
	return shaderModule;
}

void Vulkan::createGraphicsPipeline() {
	if(swapChainExtent.width == 0 || swapChainExtent.height == 0) {
		throw std::runtime_error("swapChainExtent has not been initialized.");
	}
	//TODO gonna have to rethink shaders
    auto vertShaderCode = utils::readFile("assets/shaders/shader.vert.spv");
    auto fragShaderCode = utils::readFile("assets/shaders/shader.frag.spv");
	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertShaderStageInfo, fragShaderStageInfo
	};

	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(component::model::vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription attributeDescription = {};
	attributeDescription.binding = 0;
	attributeDescription.location = 0;
	attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescription.offset = 0;

	VkPipelineVertexInputStateCreateInfo vInputInfo = {};
	vInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vInputInfo.vertexBindingDescriptionCount = 1;
	vInputInfo.vertexAttributeDescriptionCount = 1;
	vInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vInputInfo.pVertexAttributeDescriptions = &attributeDescription;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) swapChainExtent.width;
	viewport.height = (float) swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	//rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
	rasterizer.lineWidth = 1.0f;
	//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = 
		VK_COLOR_COMPONENT_R_BIT | 
		VK_COLOR_COMPONENT_G_BIT | 
		VK_COLOR_COMPONENT_B_BIT | 
		VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	/*
	* Some things can dynamically altered without recreating
	* the pipeline
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;
	*/

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
	if(
		vkCreatePipelineLayout(
			device, &pipelineLayoutInfo, nullptr, 
			&pipelineLayout) != VK_SUCCESS
		) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional
	if(
		vkCreateGraphicsPipelines(
			device, VK_NULL_HANDLE, 1, &pipelineInfo, 
			nullptr, &graphicsPipeline) != VK_SUCCESS
		) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}


void Vulkan::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if(
		vkCreateDescriptorSetLayout(
			device, &layoutInfo, nullptr, 
			&descriptorSetLayout) != VK_SUCCESS
		) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void Vulkan::createRenderPass() {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	if(
		vkCreateRenderPass(
			device, &renderPassInfo, nullptr, 
			&renderPass) != VK_SUCCESS
		) {
		throw std::runtime_error("Failed to create render pass!");
	}
}

void Vulkan::createImageViews() {
	if(swapChain == VK_NULL_HANDLE) {
		throw std::runtime_error("Swapchain can't be null!");
	}
	if(swapChainImages.size() == 0) {
		throw std::runtime_error("No swapchain images found!");
	}
	if(swapChainImageFormat == VK_FORMAT_UNDEFINED) {
		throw std::runtime_error("No swapchain image formats found!");
	}
	swapChainImageViews.resize(swapChainImages.size());
	//TODO i have no idea if setting 
	for(uint32_t i = 0; i < swapChainImages.size(); ++i) {
		VkImageViewCreateInfo cInfo = {};
		cInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		cInfo.image = swapChainImages[i];
		cInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		cInfo.format = swapChainImageFormat;
		cInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		cInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		cInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		cInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		cInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		cInfo.subresourceRange.baseMipLevel = 0;
		cInfo.subresourceRange.levelCount = 1;
		cInfo.subresourceRange.baseArrayLayer = 0;
		cInfo.subresourceRange.layerCount = 1;
		if(vkCreateImageView(
			device, &cInfo, nullptr, 
			&swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed creating a swapchain image view!");
		}
	}
}


VkSurfaceFormatKHR Vulkan::chooseSwapSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR> availableFormats) {
	for(const auto& a : availableFormats) {
		if(a.format == VK_FORMAT_B8G8R8A8_UNORM && 
			a.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return a;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR Vulkan::chooseSwapPresentMode(
	const std::vector<VkPresentModeKHR> availablePresentModes) {
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
	for(const auto& a : availablePresentModes) {
        if(a == VK_PRESENT_MODE_MAILBOX_KHR) {
            return a;
        } else if(a == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            bestMode = a;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Vulkan::chooseSwapExtent(
	const VkSurfaceCapabilitiesKHR& capabilities) {
	    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
        VkExtent2D actualExtent = {
			static_cast<uint32_t>(width), 
			static_cast<uint32_t>(height)
		};
        actualExtent.width = std::max(
			capabilities.minImageExtent.width, 
			std::min(capabilities.maxImageExtent.width, actualExtent.width)
		);
        actualExtent.height = std::max(
			capabilities.minImageExtent.height, 
			std::min(capabilities.maxImageExtent.height, actualExtent.height)
		);
        return actualExtent;
    }
}

void Vulkan::createSwapChain() {
	if(physicalDevice	== VK_NULL_HANDLE) {
		throw std::runtime_error("No physical device found!");
	}
	if(surface == VK_NULL_HANDLE) {
		throw std::runtime_error("No surface found!");
	}
	SwapChainSupportDetails details = 
		querySwapChainSupport(physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(details.formats);		
	VkPresentModeKHR presentMode = chooseSwapPresentMode(details.presentModes);
	VkExtent2D extent = chooseSwapExtent(details.capabilities);
	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if(
		details.capabilities.maxImageCount && 
		imageCount > details.capabilities.maxImageCount
	) {
		imageCount = details.capabilities.maxImageCount;
	}
	VkSwapchainCreateInfoKHR cInfo = {};
	cInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	cInfo.surface = surface;
	cInfo.minImageCount = imageCount;
	cInfo.imageFormat = surfaceFormat.format;
	cInfo.imageColorSpace = surfaceFormat.colorSpace;
	cInfo.imageExtent = extent;
	cInfo.imageArrayLayers = 1;
	cInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
//NOTE this could change if graphicsFamily != presentFamily
	cInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	cInfo.queueFamilyIndexCount = 0;
	cInfo.pQueueFamilyIndices = nullptr;
	cInfo.preTransform = details.capabilities.currentTransform;
	cInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	cInfo.presentMode = presentMode;
	cInfo.clipped = VK_TRUE;
	cInfo.oldSwapchain = VK_NULL_HANDLE;
	if(vkCreateSwapchainKHR(device, &cInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("Could not create swapchain!");
	}
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void Vulkan::createLogicalDeviceAndQueue() {
	if(physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("No physical device found!");
	}
	uint32_t qFamilyIndex  = getQueueFamilyIndex(physicalDevice);
	if(qFamilyIndex == UINT32_MAX) {
		throw std::runtime_error("No Vulkan Queues found!");
	}
	//I only need one, I think... My selection logic isn't as
	//sophisticated as the tutorials, and that mostly only uses one too
	//see https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Window_surface
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = qFamilyIndex;
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo dCreateInfo = {};
	dCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	dCreateInfo.pNext = nullptr;
	dCreateInfo.queueCreateInfoCount = 1;
	dCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	VkPhysicalDeviceFeatures features = {};
	dCreateInfo.pEnabledFeatures = &features;
	dCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	dCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if(vkCreateDevice(
		physicalDevice, &dCreateInfo, 
		nullptr, &device
	) != VK_SUCCESS) {
		throw std::runtime_error("Could not create logical device!");
	}
	if(device == VK_NULL_HANDLE) {
		throw std::runtime_error("Could not create logical device!");
	}
	//TODO, these are the same, queue selection will need to be revisited for more
	//sophistication
	vkGetDeviceQueue(device, qFamilyIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(device, qFamilyIndex, 0, &presentQueue);
	if(graphicsQueue == VK_NULL_HANDLE || 
		presentQueue == VK_NULL_HANDLE) {
		throw std::runtime_error("Could not create queues!");
	}
}

bool Vulkan::checkDeviceExtensionSupport(VkPhysicalDevice pDevice) {
   uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(
		pDevice, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
		pDevice, nullptr, &extensionCount, availableExtensions.data());
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for(const auto& extension : availableExtensions) {
		if(requiredExtensions.empty()) return true;
        requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
}

uint32_t Vulkan::getQueueFamilyIndex(VkPhysicalDevice pDevice) {
	static uint32_t index = UINT32_MAX;
	//no need to keep running through this
	if(index < UINT32_MAX) {
		return index;
	}
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		pDevice, &queueFamilyCount, nullptr);
	//can be more than one physical device,
	//let the caller determine whether proceed.
	if(!queueFamilyCount) {
		return index;
	}
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(
		pDevice, &queueFamilyCount, queueFamilies.data());
	for(uint32_t i = 0; i < queueFamilyCount; ++i) {
		auto q = queueFamilies[i];
		VkBool32 presentSupport = false;
		auto res = vkGetPhysicalDeviceSurfaceSupportKHR(
			pDevice, i, surface, &presentSupport
		);
		if(res != VK_SUCCESS) continue;
		if(q.queueCount && q.queueFlags & VK_QUEUE_GRAPHICS_BIT && presentSupport) {
			index = i;
			break;
		}
	}
	return index;
}

//NOTE, this might throw a fatal error when killing the program isn't appropriate...
SwapChainSupportDetails Vulkan::querySwapChainSupport(VkPhysicalDevice pDevice) {
	SwapChainSupportDetails details;
	if(surface == VK_NULL_HANDLE) {
		throw std::runtime_error("Surface can't be null!");
	}
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		pDevice, surface, &details.capabilities);
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		pDevice, surface, &formatCount, nullptr);
	if(formatCount == 0) {
		throw std::runtime_error("No physical device formats found!");
	}
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
		pDevice, surface, &formatCount, details.formats.data()
	);
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		pDevice, surface, &presentModeCount, nullptr
	);
	if(presentModeCount == 0) {
		throw std::runtime_error("No physical device present modes found!");
	}
	details.presentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		pDevice, surface, &presentModeCount, details.presentModes.data()
	);
    return details;
}

void Vulkan::selectSuitableDevice(const std::vector<VkPhysicalDevice> devices) {
	for(auto d : devices) {
		if(getQueueFamilyIndex(d) == UINT32_MAX) continue;
		if(!checkDeviceExtensionSupport(d)) continue;
		SwapChainSupportDetails swapChainSupport = 
			querySwapChainSupport(d);
		if(
			swapChainSupport.formats.empty() ||
			swapChainSupport.presentModes.empty()) {
			continue;
		}
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(d, &deviceProperties);
		//Selecting a descrete GPU is good enough for now
		if(deviceProperties.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			physicalDevice = d;
			break;
		}
	}
}

void Vulkan::createPhysicalDevice() {
	uint32_t dCount = 0;
	vkEnumeratePhysicalDevices(instance, &dCount, nullptr);
	if(dCount == 0) {
		throw std::runtime_error("Could not find physical devices!");
	}
	std::vector<VkPhysicalDevice> devices(dCount);
	vkEnumeratePhysicalDevices(instance, &dCount, devices.data());
	selectSuitableDevice(devices);
	if(physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("No physical devices selected!");
}

VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
	VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
	void* pUserData
) {
	std::string severity;
	if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
		severity.assign("VERBOSE");
	} else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
		severity.assign("INFO");
	} else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		severity.assign("WARNING");
	} else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		severity.assign("ERROR");
	}
	std::string type;
	if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
		type.assign("GENERAL");
	} else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
		type.assign("VALIDATION");
	} else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
		type.assign("PERFORMANCE");
	}
	//no idea how long this would need to be...
	char buff[500];
	const char* fmt = "validation layer: [%s] [%s]: %s";
	sprintf(buff, fmt, severity.c_str(), type.c_str(), pCallbackData->pMessage);
	std::cout << buff << std::endl;
	return VK_FALSE;
}

void Vulkan::createDebugger() {
	if(!debug) return;
	VkDebugUtilsMessengerCreateInfoEXT cInfo = {};
	cInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	cInfo.pNext = nullptr;
	cInfo.flags = 0;
	cInfo.messageSeverity = 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
		//VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	cInfo.messageType = 
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT; 
	cInfo.pfnUserCallback = debugCallback;
	auto vkCreateDebugUtilsMessengerEXT = 
		(PFN_vkCreateDebugUtilsMessengerEXT) 
			vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	auto res = vkCreateDebugUtilsMessengerEXT(
		instance, &cInfo, nullptr, &debugger);
	if(res != VK_SUCCESS) {
		throw std::runtime_error("Could not create debugger!");
	}
}

void Vulkan::createSurface() {
	if(instance == VK_NULL_HANDLE) {
		throw std::runtime_error("Instance can't be null!");
	}
	//don't know if this modifies window or what....
	if(
		glfwCreateWindowSurface(
			instance, window, nullptr, &surface
		) != VK_SUCCESS) {
		throw std::runtime_error("Could not create surface!");
	}
	if(surface == VK_NULL_HANDLE) {
		throw std::runtime_error("Could not create surface!");
	}
}

void Vulkan::setExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	extensions.assign(glfwExtensions, glfwExtensions + glfwExtensionCount);
	//required for layer callbacks
	if(debug) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
}

void Vulkan::setValidationLayers() {
	if(debug) {
		validationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};
	}
}

void Vulkan::createInstance() {
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Drone";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	setExtensions();
	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();

	setValidationLayers();
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if(result != VK_SUCCESS || instance == VK_NULL_HANDLE) {
		throw std::runtime_error("Could not create Vulkan Instance!");
	}
}
