#pragma once

#include <sol/sol.hpp>
#include <filesystem>

namespace REngine::Core {
	class ScriptEngine {
	public:
		void Initialize();
		void RegisterTypes();
		void RunFile(const std::filesystem::path &path);
	private:
		sol::state lua;
	};
}