#pragma once

namespace REngine::Core {
	class Store {
		Store() = default;
		Store(const Store&) = delete;
		Store &operator=(const Store&) = delete;
	public:
		static Store *instance;
	
		static Store &Inst();

		static void DropStore();
	};
}