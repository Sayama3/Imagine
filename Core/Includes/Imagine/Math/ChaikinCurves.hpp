//
// Created by Iannis on 24/06/2025.
//

#pragma once

#include "Core.hpp"
#include "Types.hpp"

namespace Imagine::Math {
	template<glm::length_t L = 3, typename T = Real, glm::qualifier Q = glm::defaultp, bool Loops = true>
	class ChaikinCurves {
	public:
		static_assert(std::is_floating_point_v<T>, "Cannot use the curves without floating points");
		using vec = glm::vec<L, T, Q>;

	public:
		class SegmentIterator {
		public:
			ChaikinCurves* curve{nullptr};
			uint64_t segmentIndex{0};
		public:
			Line<L,T,Q> GetSegment() const {
				if constexpr (Loops) {
					const auto pointCount = curve->line.size();
					const auto p1 = segmentIndex % pointCount;
					const auto p2 = (segmentIndex + 1) % pointCount;
					return Line{curve->line[p1], curve->line[p2]};
				} else {
					const auto p1 = segmentIndex;
					const auto p2 = segmentIndex + 1;
					return Line{curve->line[p1], curve->line[p2]};
				}
			}

		public:
			SegmentIterator &operator++() {segmentIndex += 1; return *this;}
			SegmentIterator operator++(int) {SegmentIterator it = *this; ++*this; return it;}
		};

		SegmentIterator begin() {return {this, 0};}
		SegmentIterator end() { if constexpr (Loops) { return {this, line.size()}; } else { return {this, line.size() - 1}; } }
	public:
		ChaikinCurves &SetUV(T u, T v);
		ChaikinCurves &SetStepCount(uint64_t stepCount);
		ChaikinCurves &AddPoint(vec point);
		ChaikinCurves &AddPoints(const vec *begin, const vec *end);
		ChaikinCurves &ClearPoints();

		std::vector<vec> CalculateChaikin();
	private:
		vec GetFirst() const {return line.front();}
		vec GetLast() const {if constexpr (Loops) return line.front(); else return line.back();}
	private:
		T u, v; // The U/V ratio
		T oneMinus;
		uint64_t steps = 1;
	public:
		std::vector<vec> line;
	};

	template<glm::length_t L, typename T, glm::qualifier Q, bool Loops>
	ChaikinCurves<L, T, Q, Loops> &ChaikinCurves<L, T, Q, Loops>::SetUV(const T _u, const T _v) {
		u = _u;
		v = _v;
		if (u < 0) u = 0;
		if (v < 0) v = 0;
		const auto combined = u + v;
		if (combined > 1) {
			const auto excess = combined - T(1);
			u -= excess * T(0.5);
			v -= excess * T(0.5);
		}

		oneMinus = T(1) - (u + v);
		return *this;
	}

	template<glm::length_t L, typename T, glm::qualifier Q, bool Loops>
	ChaikinCurves<L, T, Q, Loops> &ChaikinCurves<L, T, Q, Loops>::SetStepCount(const uint64_t stepCount) {
		steps = stepCount;
		return *this;
	}

	template<glm::length_t L, typename T, glm::qualifier Q, bool Loops>
	ChaikinCurves<L, T, Q, Loops> &ChaikinCurves<L, T, Q, Loops>::AddPoint(const vec point) {
		line.push_back(point);
		return *this;
	}

	template<glm::length_t L, typename T, glm::qualifier Q, bool Loops>
	ChaikinCurves<L, T, Q, Loops> &ChaikinCurves<L, T, Q, Loops>::AddPoints(const vec *begin, const vec *end) {
		line.insert(line.end(), begin, end);
		return *this;
	}

	template<glm::length_t L, typename T, glm::qualifier Q, bool Loops>
	ChaikinCurves<L, T, Q, Loops> &ChaikinCurves<L, T, Q, Loops>::ClearPoints() {
		line.clear();
	}

	template<glm::length_t L, typename T, glm::qualifier Q, bool Loops>
	std::vector<typename ChaikinCurves<L, T, Q, Loops>::vec> ChaikinCurves<L, T, Q, Loops>::CalculateChaikin() {
		if (line.empty()) return {};

		ChaikinCurves result = *this;
		std::vector<vec> newLine;
		while (result.steps != 0) {
			newLine.clear();
			newLine.reserve(result.line.size() + 2);
			newLine.push_back(result.GetFirst());
			const auto beg = result.begin();
			const auto end = result.end();
			for (auto it = beg; it != end; ++it) {
				const vec p1 = it.GetSegment().GetPoint(u);
				const vec p2 = it.GetSegment().GetPoint(T(1)-v);

				newLine.push_back(p1);
				newLine.push_back(p2);
			}

			if constexpr (!Loops) newLine.push_back(result.GetLast());

			--result.steps;
			std::swap(result.line, newLine);
		}

		return std::move(result.line);
	}
} // namespace Imagine::Math
