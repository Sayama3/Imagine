//
// Created by ianpo on 26/02/2024.
//

#pragma once

#include "BoundingObject.hpp"
#include "Basics.hpp"

namespace Imagine
{

	struct BoundingSphere : public BoundingObject<BoundingSphere>
	{
	public:
		inline BoundingSphere() = default;
		inline ~BoundingSphere() = default;
		inline BoundingSphere(const BoundingSphere& one) = default;

		template<class ListVec3>
		BoundingSphere(const ListVec3& points);
		BoundingSphere(const Vec3& center, Real radius);
		BoundingSphere(const BoundingSphere& one,const BoundingSphere& two);
	public:
		[[nodiscard]] bool Overlaps(const BoundingSphere& other) const;
		[[nodiscard]] Real GetSize() const;
		void Grow(const Vec3& point);
		void Grow(const BoundingSphere& other);
		Real GetGrowth(BoundingSphere other) const;
	private:
		void Reset(const Vec3& point);
		Vec3 m_Center = {0,0,0};
		Real m_Radius = -1;
	};

	template<class ListVec3>
	BoundingSphere::BoundingSphere(const ListVec3 &points)
	{
		MGN_PROFILE_FUNCTION();
		bool init = false;
		for(const Vec3& point : points)
		{
			if(init)
			{
				Grow(point);
			}
			else
			{
				Reset(point);
				init = true;
			}
		}
	}

} // namespace Imagine

