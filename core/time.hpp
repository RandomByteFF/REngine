#pragma once

#include <chrono>

namespace REngine::Core {
	class Time {
		inline static std::chrono::high_resolution_clock::time_point start;
		inline static std::chrono::high_resolution_clock::time_point last;
		inline static float dt = 0;
	public:
		static void Start();
		static void Tick();
		static float Delta();
	};
}