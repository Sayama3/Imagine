//
// Created by ianpo on 27/05/2025.
//

#pragma once

namespace Imagine::Core {

	struct DeletionQueue {
		std::deque<std::function<void()>> deletors;

		void push_function(std::function<void()> &&function) {
			deletors.push_back(std::move(function));
		}

		void flush() {
			// reverse iterate the deletion queue to execute all the functions
			for (auto it = deletors.rbegin(); it != deletors.rend(); ++it) {
				(*it)(); // call functors
			}

			deletors.clear();
		}
	};
} // namespace Imagine::Core
