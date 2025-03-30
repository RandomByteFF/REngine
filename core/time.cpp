#include "time.hpp"

namespace REngine::Core {
	void Time::Start() {
		start = std::chrono::high_resolution_clock::now();
		last = start;
	}

	void Time::Tick() {
		auto temp = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::seconds::period>(temp - last).count();
		last = temp;
	}

	float Time::Delta() {
		return dt;
	}
}