#pragma once

namespace Util
{
	template <typename Type>
	class ObjectPool
	{
	public:

		typedef size_t Index;

		ObjectPool(size_t initialSize = 16)
			: _maxIndex(0)
		{
			_array.resize(initialSize);
			for (size_t i = initialSize - 1; i >= 0; --i)
			{
				_free.push(i);
			}
		}

		Index Create()
		{
			if (_free.empty())
			{
				Index firstIndex = _array.size();
				_array.resize(_array.size() * 2);

				for (size_t i = _array.size() - 1; i >= firstIndex; --i)
				{
					_free.push(i);
				}
			}

			Index newIndex = _free.top();
			_free.pop();

			if (newIndex > _maxIndex)
			{
				_maxIndex = newIndex;
			}

			return newIndex;
		}

		void Free(Index index)
		{
#if JE_Debug
			for (Index& ind : _free)
			{
				JE_Assert(index != ind);
			}
#endif
			// Maxindex does not get updated here purposefully.

			_free.push(index);
		}

		Type& Get(Index index)
		{
#if JE_Debug
			for (Index& ind : _free)
			{
				JE_Assert(index != ind);
			}
#endif

			return _array[index];
		}

		Type* GetArray()
		{
			return _array.data();
		}

		Index GetMaxIndex()
		{
			return _maxIndex;
		}

	private:

		std::vector<Type> _array;
		std::stack<Index> _free;
		Index _maxIndex;
	};
}