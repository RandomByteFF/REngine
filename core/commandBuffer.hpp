#pragma once


namespace REngine::Core {
	class CommandBuffer {
		vk::CommandBuffer commandBuffer;
		
	public:
		void Create();
		void Reset();
		void Begin();
		void End();

		vk::CommandBuffer &GetBuffer();
		
		static vk::CommandBuffer BeginSingleTimeCommands();
		static void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);
	};
}