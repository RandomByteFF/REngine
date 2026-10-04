#pragma once

#include <memory>
namespace REngine::Common {
	template<class T>
	class NullChecker {
		T *object;
		NullChecker(T *object) { this->object = object; }
	public:
		static std::shared_ptr<NullChecker> New(T *self) {
			auto r = std::shared_ptr<NullChecker<T>>(new NullChecker<T>(self));
			return r;
		}
		T &Object() {
			return object;
		}
	};
}