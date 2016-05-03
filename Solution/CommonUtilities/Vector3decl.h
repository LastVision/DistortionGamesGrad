#pragma once

#include "Vector2.h"

namespace CU
{

	template<typename T>
	class Vector3
	{
	public:
		Vector3();
		Vector3(T aX, T aY, T aZ);
		Vector3(Vector2<T> aVector, T aZ);
		~Vector3();
		Vector2<T> GetVector2()
		{
			return Vector2<T>(x, y);
		}

		T x;
		T y;
		T z;
	};
}