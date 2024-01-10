#pragma once

#include <mutex>
#include <memory>
#include <functional>

template <typename T>
class Singleton {
public:
	template <typename... Args>
	static T* initialize(Args&&... args) {
		static std::once_flag fInit;
		std::call_once(fInit, []() {
			std::mem_fn(&std::unique_ptr<T>::reset)(&instance, new T(std::forward<Args>(args)...));
		});
		return instance.get();
	}

	static void destroy() {
		instance.reset();
	}
private:
	static std::unique_ptr<T> instance;
};

template <typename T>
std::unique_ptr<T> Singleton<T>::instance;