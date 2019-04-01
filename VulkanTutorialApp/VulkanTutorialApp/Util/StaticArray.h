#pragma once

namespace Util
{
	template <typename ObjectType, size_t size>
	class StaticArray
	{
	public:

		static const size_t TOTAL_SIZE = size;

		StaticArray(size_t initialSize = TOTAL_SIZE)
			: _currentSize(initialSize)
		{
		}

		JE_Inline void Clear()
		{
			for (size_t i = 0; i < _currentSize; ++i)
			{
				_array[i].~ObjectType();
			}
			_currentSize = 0;
		}

		JE_Inline ObjectType& Push(const ObjectType& val)
		{
			JE_Assert(_currentSize < TOTAL_SIZE);
			_array[_currentSize] = val;
			ObjectType& toRet = _array[_currentSize];
			++_currentSize;
			return toRet;
		}

		JE_Inline ObjectType Pop()
		{
			JE_Assert(_currentSize > 0);
			_array[_currentSize].~ObjectType();
			ObjectType& toRet = _array[_currentSize];
			--_currentSize;
			return toRet;
		}

		JE_Inline void Set(const ObjectType& val, size_t index)
		{
			JE_Assert(index < _currentSize);
			_array[index] = val;
		}

		JE_Inline ObjectType& Get(size_t index) 
		{ 
			JE_Assert(index < _currentSize);
			return _array[index];
		}

		JE_Inline const ObjectType& Get(size_t index) const
		{
			JE_Assert(index < _currentSize);
			return _array[index];
		}

		JE_Inline ObjectType GetCopy(size_t index) const
		{
			JE_Assert(index < _currentSize);
			return _array[index];
		}

		ObjectType& operator[](size_t index)
		{
			return Get(index);
		}

		ObjectType operator[](size_t index) const
		{
			return GetCopy(index);
		}

		JE_Inline bool IsFull() const
		{
			return _currentSize == TOTAL_SIZE;
		}

		JE_Inline bool IsEmpty() const
		{
			return _currentSize == 0;
		}

		JE_Inline size_t GetCurrentSize() const { return _currentSize; }
		JE_Inline size_t GetTotalSize() const { return TOTAL_SIZE; }

	private:

		ObjectType _array[size] = {};
		size_t _currentSize;
	};
}