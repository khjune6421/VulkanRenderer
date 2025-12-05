#pragma once

class Renderer
{
	std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> m_window{ nullptr, glfwDestroyWindow };
	uint32_t m_width = 800;
	uint32_t m_height = 600;

	// Initialized Vulkan objects
	vkb::Instance m_instance = {};
	vkb::InstanceDispatchTable m_instanceDispatchTable = {};
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	vkb::PhysicalDevice m_physicalDevice = {};
	vkb::Device m_device = {};
	vkb::DispatchTable m_deviceDispatchTable = {};
	vkb::Swapchain m_swapchain = {};

	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	VkQueue m_presentQueue = VK_NULL_HANDLE;
	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;

	void InitializeWindow();
	void InitializeVulkan();
	void Cleanup();

	void CreateInstance();
	void CreateSurface();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapchain();

	void GetQueues();
	void CreateRenderPass();
	void CreateGraphicsPipeline();

public:
	Renderer(uint32_t width = 800, uint32_t height = 600);
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	void Run();
};