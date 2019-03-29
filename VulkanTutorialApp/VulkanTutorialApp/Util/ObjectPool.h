#pragma once

namespace Util
{
	template <typename Type>
	class ObjectPool
	{
	public:

		typedef size_t Index;
		static const Index BAD_INDEX = std::numeric_limits<Index>::max();

		ObjectPool(size_t initialSize = 16)
			: _maxIndex(0)
			, _currentIndex(0)
		{
			ResizeArrays(initialSize);
		}

		Index Create()
		{
			Index ret = _currentIndex;
			JE_Assert(!_used[ret]);
			JE_Assert(ret != BAD_INDEX);

			UpdateCurrentIndexOnCreate();

			UpdateMaxIndexOnCreate(ret);

			_used[ret] = true;

			return ret;
		}

		void Free(Index index)
		{
			JE_Assert(_used[index]);
			JE_Assert(index != BAD_INDEX);

			_used[index] = false;
			UpdateCurrentIndexOnFree(index);
			UpdateMaxIndexOnFree(index);
		}

		Type& Get(Index index)
		{
			JE_Assert(_used[index]);
			JE_Assert(index != BAD_INDEX);
			return _array[index];
		}

		Type* GetArray()
		{
			return _array.data();
		}

		Index GetMaxIndex()
		{
			JE_Assert(_used[_maxIndex]);
			JE_Assert(_maxIndex != BAD_INDEX);
			return _maxIndex;
		}

		bool IsEmpty()
		{
			return _maxIndex == 0 && !_used[_maxIndex];
		}

		Index Copy(ObjectPool<Type>& poolFrom, Index sourceIndex)
		{
			Index myNewIndex = Create();
			_array[myNewIndex] = poolFrom._array[sourceIndex];
			return myNewIndex;
		}

		Index Move(ObjectPool<Type>& poolFrom, Index oldIndex)
		{
			if (&poolFrom == this)
			{
				return oldIndex;
			}

			Index myNewIndex = Create();
			_array[myNewIndex] = poolFrom._array[oldIndex];
			poolFrom.Free(oldIndex);

			return myNewIndex;
		}

	private:

		JE_Inline size_t GetSize() { return _array.size(); }
		JE_Inline size_t GetLastIndex() { return GetSize() - 1; }

		void ResizeArrays(size_t newSize)
		{
			_array.resize(newSize);
			_used.resize(newSize);
		}

		void UpdateCurrentIndexOnCreate()
		{
			if (_currentIndex == GetLastIndex())
				_currentIndex = 0;
			else
				++_currentIndex;

			const Index startIndex = _currentIndex;
			while (_used[_currentIndex])
			{
				if (_currentIndex < GetLastIndex())
				{
					++_currentIndex;
				}
				else if(_currentIndex == startIndex)
				{
					// No free index was found. Enlarge arrays and put current index at oldSize.
					const size_t oldSize = GetSize();
					Enlarge();
					_currentIndex = oldSize;
					JE_Assert(_maxIndex == _currentIndex - 1);

					break;
				}
				else
				{
					_currentIndex = 0;
				}
			}
		}

		void UpdateMaxIndexOnCreate(const Index acquiredIndex)
		{
			if (acquiredIndex > _maxIndex)
			{
				_maxIndex = acquiredIndex;
			}
		}

		void UpdateCurrentIndexOnFree(const Index freedIndex)
		{
			if (freedIndex < _currentIndex)
			{
				_currentIndex = freedIndex;
			}
		}

		void UpdateMaxIndexOnFree(const Index freedIndex)
		{
			if (freedIndex == _maxIndex && _maxIndex > 0)
			{
				while (!_used[_maxIndex] && _maxIndex > 0)
					--_maxIndex;
			}
		}

		void Enlarge()
		{
			ResizeArrays(2 * GetSize());
		}


		std::vector<Type> _array;
		std::vector<bool> _used;

		Index _currentIndex;
		Index _maxIndex;
	};
}