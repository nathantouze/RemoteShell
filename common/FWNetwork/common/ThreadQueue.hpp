
#ifndef _THREAD_QUEUE_
#define _THREAD_QUEUE_

#include <mutex>
#include <deque>
#include <condition_variable>

namespace FWNetwork {

	template<typename T>
	class ThreadQueue {
		public:
			ThreadQueue() = default;

			ThreadQueue(const ThreadQueue<T>&) = delete;

			virtual ~ThreadQueue() { 
				clear();
			};

			const T& front() {
				std::scoped_lock lock(_mutexList);
				return _dequeList.front();
			};

			const T& back() {
				std::scoped_lock lock(_mutexList);
				return _dequeList.back();
			};

			T pop_front() {
				std::scoped_lock lock(_mutexList);
				auto t = std::move(_dequeList.front());
				_dequeList.pop_front();
				return t;
			};

			T pop_back() {
				std::scoped_lock lock(_mutexList);
				auto t = std::move(_dequeList.back());
				_dequeList.pop_back();
				return t;
			};

			void push_back(const T& item) {
				std::scoped_lock lock(_mutexList);
				_dequeList.emplace_back(std::move(item));
				std::unique_lock<std::mutex> ul(_mutexBlock);
				_conditionBlock.notify_one();
			};

			void push_front(const T& item) {
				std::scoped_lock lock(_mutexList);
				_dequeList.emplace_front(std::move(item));
				std::unique_lock<std::mutex> ul(_mutexBlock);
				_conditionBlock.notify_one();
			};

			bool empty() {
				std::scoped_lock lock(_mutexList);
				return _dequeList.empty();
			};

			size_t count() {
				std::scoped_lock lock(_mutexList);
				return _dequeList.size();
			};

			void clear() {
				std::scoped_lock lock(_mutexList);
				_dequeList.clear();
			};

			void wait() {
				while (empty()) {
					std::unique_lock<std::mutex> lock(_mutexBlock);
					_conditionBlock.wait(lock);
				}
			};

		protected:
			std::mutex _mutexList;
			std::mutex _mutexBlock;
			std::deque<T> _dequeList;
			std::condition_variable _conditionBlock;
	};

}; //namespace FWNetwork

#endif