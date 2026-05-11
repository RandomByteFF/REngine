#include "scriptEngine.hpp"

namespace REngine::Core {
	void ScriptEngine::Initialize() {
		lua.open_libraries(
			sol::lib::base,
			sol::lib::math,
			sol::lib::string,
			sol::lib::table,
			sol::lib::io
		);
	}

	void ScriptEngine::RunFile(const std::filesystem::path &path) {
		auto result = lua.safe_script_file(path.string());
		if (!result.valid()) {
			sol::error err = result;
			throw std::runtime_error(err.what());
		}
	}

	void ScriptEngine::RegisterTypes() {
		lua.set_function("engine_print", [this]() {std::cout << "Hello there!";});
	}
}