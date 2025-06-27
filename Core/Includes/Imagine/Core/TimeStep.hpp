//
// Created by ianpo on 25/07/2023.
//

#pragma once

typedef double TimeType;

namespace Imagine::Core {
	struct TimeStep {
	public:
		TimeStep(const TimeType timeInSeconds = 1.0) : m_Time(timeInSeconds) {}

		inline operator TimeType() const { return m_Time; }

		inline TimeType GetSeconds() const { return m_Time; }
		inline TimeType GetMilliSeconds() const { return m_Time * 1000.0; }

		template<typename T>
		inline T as() const {
			return T(m_Time);
		}

	private:
		TimeType m_Time;
	};

} // namespace Imagine::Core
