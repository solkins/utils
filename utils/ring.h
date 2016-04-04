/*
 * ring.h
 *
 *  Created on: Jun 28, 2013
 *      Author: joe
 */

#ifndef RINGQUEUE_H_
#define RINGQUEUE_H_

#include "type.h"

namespace Joe
{

	template<class T>
	class ring
	{
	public:
		ring()
		{
			clear();
		}

		bool push(const T& t)
		{
			if (full())
				return false;

			_ring[in_index] = t;
			++in_count;
			++in_index;
			return true;
		}

		T pop()
		{
			T result = NULL;
			if (in_count != out_count)
			{
				result = _ring[out_index];
				++out_count;
				++out_index;
			}
			return result;
		}

		bool full()
		{
			return (unsigned char)(in_count + 1) == out_count;
		}

		bool empty()
		{
			return in_count == out_count;
		}

		void clear()
		{
			in_count = 0;
			in_index = 0;
			out_count = 0;
			out_index = 0;
		}

	private:
		T _ring[256];
		uint8 in_count;
		uint8 in_index;
		uint8 out_count;
		uint8 out_index;
	};

} /* namespace Joe */

#endif /* RINGQUEUE_H_ */
