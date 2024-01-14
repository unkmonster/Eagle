#pragma once

#include <mutex>
#include <memory>
#include <functional>
#include <shared_mutex>

template <typename T>
class Singleton {
public:
	template <typename... Args>
	static T* initialize(Args&&... args) {
		static std::once_flag fInit;
		std::call_once(fInit, [](Args&&... args) {
			std::mem_fn(&std::unique_ptr<T>::reset)(&instance, new T(std::forward<Args>(args)...));
		}, std::forward<Args>(args)...);

		std::shared_lock<std::shared_mutex> lock(smutex);
		return instance.get();
	}

	static void destroy() {
		std::lock_guard<std::shared_mutex> lock(smutex);
		instance.reset();
	}

	static std::shared_mutex& mutex() {
		return smutex;
	}
private:
	static std::unique_ptr<T> instance;
	static std::shared_mutex smutex;
};

template <typename T>
std::unique_ptr<T> Singleton<T>::instance;

template <typename T>
std::shared_mutex Singleton<T>::smutex;