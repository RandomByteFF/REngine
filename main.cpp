#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

class VulkanApp {
public:
	void Run() {
		InitVulkan();
	}
private:
	void InitVulkan() {

	}

	void MainLoop() {

	}

	void Cleanup() {

	}
};

int main() {
	VulkanApp app;

	try
	{
		app.Run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;	
}