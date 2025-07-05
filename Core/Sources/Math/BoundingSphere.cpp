//
// Created by ianpo on 26/02/2024.
//

#include "Imagine/Math/BoundingSphere.hpp"
#include "Imagine/Core/InternalCore.hpp"

namespace Imagine::Core {
	bool BoundingSphere::Overlaps(const BoundingSphere &other) const {
		if (m_Radius < 0 || other.m_Radius < 0) return false;

		// Calculate the squared distance between the two centers
		Real distSquared = Math::Magnitude2(this->m_Center - other.m_Center);

		// Calculate the squared sum of the radius
		Real radiusSumSquared = (this->m_Radius + other.m_Radius) * (this->m_Radius + other.m_Radius);

		// Return true if the squared distance is less than the squared sum of their radius
		return distSquared < radiusSumSquared; // The spheres do not overlap
	}

	Real BoundingSphere::GetSize() const {
		return m_Radius;
	}
	BoundingSphere::BoundingSphere(const Vec3 &center, Real radius) :
		m_Center(center), m_Radius(radius) {}

	BoundingSphere::BoundingSphere(const BoundingSphere &one, const BoundingSphere &two) :
		m_Center(one.m_Center), m_Radius(one.m_Radius) {
		Grow(two);
	}

	void BoundingSphere::Grow(const Vec3 &point) {
		Vec3 direction = point - m_Center;
		Real distance = Math::Magnitude2(direction);
		if (Math::Magnitude2(point - m_Center) <= Math::Pow2(m_Radius)) return;
		distance = Math::Sqrt(distance);
		direction = direction / distance;

		Real outside = distance - m_Radius;
		Real grow = outside * (Real) 0.5;

		m_Center += direction * grow;
		m_Radius += grow;
	}

	void BoundingSphere::Grow(const BoundingSphere &other) {
		if (other.m_Radius < 0) {
			return;
		}
		else if (m_Radius < 0) {
			m_Center = other.m_Center;
			m_Radius = other.m_Radius;
		}
		else {
			Vec3 centerOffset = other.m_Center - m_Center;
			Real distance = Math::Magnitude2(centerOffset);
			Real radiusDiff = other.m_Radius - m_Radius;

			// No need to Sqrt yet so we do a multiplication.
			if (radiusDiff * radiusDiff >= distance) {
				if (m_Radius < other.m_Radius) {
					m_Center = other.m_Center;
					m_Radius = other.m_Radius;
				}
			}
			else {
				distance = Math::Sqrt(distance);
				auto oldRadius = m_Radius;
				m_Radius = (m_Radius + other.m_Radius + distance) * ((Real) 0.5);
				if (distance > 0) {
					m_Center += (centerOffset * ((m_Radius - oldRadius) / distance));
				}
			}
		}
	}

	Real BoundingSphere::GetGrowth(BoundingSphere other) const {
		other.Grow(*this);
		return other.GetSize() - GetSize();
	}

	void BoundingSphere::Reset(const Vec3 &point) {
		m_Center = point;
		m_Radius = 0;
	}
} // namespace Imagine::Core
