#ifndef MUD_VECTOR
#define MUD_VECTOR

#include <cmath>

#undef min
#undef max

namespace mud
{
	template<typename T, unsigned int Size>
	struct Vector
	{
		T components[Size] = { 0 };

		float magnitudeSq() const
		{
			float magSq = 0.0f;
			for(unsigned int idx = 0; idx < Size; idx++)
				magSq += components[idx] * components[idx];
			return magSq;
		}

		float magnitude() const
		{
			return std::sqrt(magnitudeSq());
		}

		float dot(const Vector<T, Size> & v) const
		{
			float dot = 0.0f;
			for(unsigned int idx = 0; idx < Size; idx++)
				dot += components[idx] * v.components[idx];
			return dot;
		}

		Vector<T, Size> normal() const
		{
			return *this / magnitude();
		}

		Vector<T, Size> & normalise()
		{
			return *this = normal();
		}

		Vector<T, Size> & limit(float newMagnitude)
		{
			float m = magnitude();
			for(unsigned int idx = 0; idx < Size; idx++)
				components[idx] += components[idx] * newMagnitude / m;
			return *this;
		}

		T & operator[](unsigned int idx)
		{
			return components[idx];
		}

		const T & operator[](unsigned int idx) const
		{
			return components[idx];
		}

		Vector<T, Size> & operator=(const Vector<T, Size> & rhs)
		{
			for(unsigned int idx = 0; idx < Size; idx++)
				components[idx] = rhs.components[idx];
			return *this;
		}

		Vector<T, Size> & operator+=(const Vector<T, Size> & rhs)
		{
			for(unsigned int idx = 0; idx < Size; idx++)
				components[idx] += rhs.components[idx];
			return *this;
		}

		Vector<T, Size> & operator-=(const Vector<T, Size> & rhs)
		{
			for(unsigned int idx = 0; idx < Size; idx++)
				components[idx] -= rhs.components[idx];
			return *this;
		}

		Vector<T, Size> & operator*=(const Vector<T, Size> & rhs)
		{
			for(unsigned int idx = 0; idx < Size; idx++)
				components[idx] *= rhs.components[idx];
			return *this;
		}

		Vector<T, Size> & operator/=(const Vector<T, Size> & rhs)
		{
			for(unsigned int idx = 0; idx < Size; idx++)
				components[idx] /= rhs.components[idx];
			return *this;
		}

		Vector<T, Size> & operator*=(float s)
		{
			for(unsigned int idx = 0; idx < Size; idx++)
				components[idx] *= s;
			return *this;
		}

		Vector<T, Size> & operator/=(float s)
		{
			for(unsigned int idx = 0; idx < Size; idx++)
				components[idx] /= s;
			return *this;
		}

		Vector<T, Size> operator-() const
		{
			return *this * -1.0f;
		}

		static Vector<T, Size> min(const Vector<T, Size> & a, const Vector<T, Size> & b)
		{
			Vector<T, Size> min;
			for(unsigned int idx = 0; idx < Size; idx++)
			{
				min[idx] = a.components[idx] < b.components[idx]
					? a.components[idx]
					: b.components[idx];
			}
			return min;
		}

		static Vector<T, Size> max(const Vector<T, Size> & a, const Vector<T, Size> & b)
		{
			Vector<T, Size> max;
			for(unsigned int idx = 0; idx < Size; idx++)
			{
				max[idx] = a.components[idx] > b.components[idx]
					? a.components[idx]
					: b.components[idx];
			}
			return max;
		}

		template <typename _T>
		explicit operator Vector<_T, Size> () const
		{
			Vector<_T, Size> converted;
			for (unsigned int idx = 0; idx < Size; idx++)
				converted[idx] = static_cast<_T>(components[idx]);
			return min;
		}
	};

	template<typename T, int Size>
	inline Vector<T, Size> operator+(Vector<T, Size> lhs, const Vector<T, Size> & rhs)
	{
		return lhs += rhs;
	}

	template<typename T, int Size>
	inline Vector<T, Size> operator-(Vector<T, Size> lhs, const Vector<T, Size> & rhs)
	{
		return lhs -= rhs;
	}

	template<typename T, int Size>
	inline Vector<T, Size> operator*(Vector<T, Size> lhs, const Vector<T, Size> & rhs)
	{
		return lhs *= rhs;
	}

	template<typename T, int Size>
	inline Vector<T, Size> operator/(Vector<T, Size> lhs, const Vector<T, Size> & rhs)
	{
		return lhs /= rhs;
	}

	template<typename T, int Size>
	inline Vector<T, Size> operator*(Vector<T, Size> lhs, float s)
	{
		return lhs *= s;
	}

	template<typename T, int Size>
	inline Vector<T, Size> operator/(Vector<T, Size> lhs, float s)
	{
		return lhs /= s;
	}

	template<typename T, int Size>
	inline Vector<T, Size> operator*(float s, Vector<T, Size> rhs)
	{
		return rhs *= s;
	}

	template<typename T, int Size>
	inline Vector<T, Size> operator/(float s, Vector<T, Size> rhs)
	{
		return rhs /= s;
	}

	template<typename T, int Size>
	inline bool operator==(const Vector<T, Size> & lhs, const Vector<T, Size> & rhs)
	{
		for(unsigned int idx = 0; idx < Size; idx++)
			if(lhs.components[idx] != rhs.components[idx])
				return false;
		return true;
	}

	template<typename T, int Size>
	inline bool operator!=(const Vector<T, Size> & lhs, const Vector<T, Size> & rhs)
	{
		return !(lhs == rhs);
	}
}

#endif