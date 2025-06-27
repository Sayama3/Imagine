//
// Created by ianpo on 30/01/2025.
//

#pragma once


#include "Basics.hpp"
#include "Geometry.hpp"

namespace Imagine::Math {
	template<typename Iter, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	std::vector<glm::vec<2, T, Q> > JarvisConvexShell(Iter begin, Iter end) {
		if (begin == end) return {};
		std::vector<glm::vec<2, T, Q> > shell;
		uint64_t count{0};
		glm::vec<2, T, Q> first = *begin;
		for (auto it = begin; it != end; ++it) {
			const glm::vec<2, T, Q> &p = *it;
			++count;
			if (p.x < first.x) {
				first = p;
			} else if (p.x == first.x && p.y < first.y) {
				first = p;
			}
		}

		glm::vec<2, T, Q> current = first;
		glm::vec<2, T, Q> next = first;
		glm::vec<2, T, Q> direction = {0, 1};
		shell.push_back(first);

		for (int i = 0; i < count * count; ++i) {
			next = *std::min_element(begin, end, [direction, current](const glm::vec<2, T, Q> &a, const glm::vec<2, T, Q> &b) {
					if (a == current) return false;
					if (b == current) return true;
					const auto cToA = a - current;
					const auto cToB = b - current;
					const auto angleA = Math::SignedAngle(direction, cToA);
					const auto angleB = Math::SignedAngle(direction, cToB);
					if (angleA == angleB) {
						return Math::Magnitude(cToA) > Math::Magnitude(cToB);
					}
					return angleA < angleB;
				}
			);

			if (next == first) {
				break;
			}
			shell.push_back(next);
			direction = next - current;
			current = next;
		}

		return shell;
	}

	template<typename Iter, typename T = Real, glm::qualifier Q = glm::qualifier::defaultp>
	std::list<glm::vec<2, T, Q>> GrahamScanConvexShell(Iter begin, Iter end) {
		if (begin == end) return {};
		using List = std::list<glm::vec<2, T, Q>>;
		using ListIter = typename List::iterator;
		List shell(begin, end);

		glm::vec<2, T, Q> center = Math::CalculateCenter<Iter, 2>(begin, end);

		shell.sort([center](const glm::vec<2, T, Q>& a, const glm::vec<2, T, Q>& b) {
			const glm::vec<2, T, Q> cToA = a - center;
			const glm::vec<2, T, Q> cToB = b - center;
			const T angleA = Math::SignedAngle(glm::vec<2, T, Q>{1,0}, cToA);
			const T angleB = Math::SignedAngle(glm::vec<2, T, Q>{1,0}, cToB);
			if (angleA == angleB) {
				return Math::Magnitude(cToA) < Math::Magnitude(cToB);
			}
			return angleA < angleB;
		});

		ListIter sInit = shell.begin();
		ListIter pivot = sInit;
		bool advance = false;
		do {
			ListIter prev = pivot == shell.begin() ? shell.end(): pivot;
			--prev;
			ListIter next = pivot;
			++next;
			if (next == shell.end()) next = shell.begin();

			if (Math::IsTriangleOriented(*prev, *pivot, *next)) {
				pivot = next;
				advance = true;
			} else {
				sInit = prev;
				shell.erase(pivot);
				pivot = sInit;
				advance = false;
			}
		} while ((pivot != sInit || advance == false) && !shell.empty());

		// return std::list<glm::vec<2, T, Q>>(shell.begin(), shell.end());
		return shell;
	}
}
