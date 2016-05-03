#pragma once

namespace CU
{
	template<typename T>

	class Vector2
	{
	public:
		Vector2();
		Vector2(T aX, T aY);
		~Vector2();

		Vector2<T> operator-()
		{
			Vector2<T> tmp = *this;
			tmp.x = -x;
			tmp.y = -y;
			return tmp;
		}

		T x;
		T y;
	};
}