#include "store.hpp"

namespace REngine::Core {
	Store &Store::Inst() {
		if (!instance) new Store();
		return *instance;
	}

	void Store::DropStore() {
		delete instance;
		instance = nullptr;
	} 
}
