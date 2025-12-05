#include "stdafx.h"
#include "Renderer.h"

using namespace std;
using namespace vkb;

void Renderer::InitializeWindow()
{
	if (!glfwInit()) cerr << "Failed to initialize GLFW" << endl;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_window.reset(glfwCreateWindow(static_cast<int>(m_width), static_cast<int>(m_height), "Vulkan Renderer", nullptr, nullptr));
}

void Renderer::InitializeVulkan()
{
	CreateInstance();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapchain();

	GetQueues();
	CreateRenderPass();
}

void Renderer::Cleanup()
{
}

void Renderer::CreateInstance()
{
	InstanceBuilder builder;
	const Result result = builder
		.set_app_name("Vulkan Renderer")
		.set_engine_name("No Engine")
		.request_validation_layers(true)
		.require_api_version(1, 0, 0)
		.use_default_debug_messenger()
		.build();
	if (!result) cerr << "Failed to create Vulkan instance: " << result.error().message() << endl;
	m_instance = result.value();
	m_instanceDispatchTable = m_instance.make_table();
}

void Renderer::CreateSurface()
{
	const VkResult vk_result = glfwCreateWindowSurface(m_instance.instance, m_window.get(), nullptr, &m_surface);
	if (vk_result != VK_SUCCESS) cerr << "Failed to create window surface: " << vk_result << endl;
}

void Renderer::PickPhysicalDevice()
{
	PhysicalDeviceSelector selector{ m_instance };
	const Result result = selector
		.set_minimum_version(1, 0)
		.set_surface(m_surface)
		.select();
	if (!result) cerr << "Failed to select physical device: " << result.error().message() << endl;
	m_physicalDevice = result.value();
}

void Renderer::CreateLogicalDevice()
{
	const DeviceBuilder builder{ m_physicalDevice };
	const Result result = builder.build();
	if (!result) cerr << "Failed to create logical device: " << result.error().message() << endl;
	m_device = result.value();
	m_deviceDispatchTable = m_device.make_table();
}

void Renderer::CreateSwapchain()
{
	SwapchainBuilder builder{ m_physicalDevice, m_device, m_surface };
	const Result result = builder.set_old_swapchain(m_swapchain).build();
	if (!result) cerr << "Failed to create swapchain: " << result.error().message() << endl;
	vkb::destroy_swapchain(m_swapchain);
	m_swapchain = result.value();
}

void Renderer::GetQueues()
{
	Result result = m_device.get_queue(vkb::QueueType::graphics);
	if (!result) cerr << "Failed to get graphics queue: " << result.error().message() << endl;
	m_graphicsQueue = result.value();

	result = m_device.get_queue(vkb::QueueType::present);
	if (!result) cerr << "Failed to get present queue: " << result.error().message() << endl;
	m_presentQueue = result.value();
}

void Renderer::CreateRenderPass()
{
	const VkAttachmentDescription colorAttachment
	{
		.format = m_swapchain.image_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};

	const VkAttachmentReference colorAttachmentRef
	{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};

	const VkSubpassDescription subpass
	{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentRef,
	};

	const VkSubpassDependency dependency
	{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	};

	const VkRenderPassCreateInfo renderPassInfo
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &colorAttachment,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};

	const VkResult vk_result = m_deviceDispatchTable.createRenderPass(&renderPassInfo, nullptr, &m_renderPass);
	if (vk_result != VK_SUCCESS) cerr << "Failed to create render pass: " << vk_result << endl;
}

void Renderer::CreateGraphicsPipeline()
{
}

Renderer::Renderer(uint32_t width, uint32_t height) : m_width(width), m_height(height)
{
	InitializeWindow();
	InitializeVulkan();
}

Renderer::~Renderer()
{
	Cleanup();
}

void Renderer::Run()
{
}
