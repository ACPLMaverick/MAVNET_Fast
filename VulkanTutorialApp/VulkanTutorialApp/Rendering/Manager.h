#pragma once

namespace Rendering
{
	template
		<
		class Key, 
		class Value, 
		class ValueWrapper = Value,
		class InitializationData = Util::NullType
		>
	class Manager
	{
	public:

		Manager() { }
		virtual ~Manager() 
		{
			JE_Assert(_memory.size() == 0);
			JE_Assert(_map.size() == 0);
		}

		// Implemented an empty function for convenience.
		virtual void Initialize() { }
		// Cleanup and destroy all cached resources.
		virtual void Cleanup()
		{
			for (Value value : _memory)
			{
				value.Cleanup();
			}
			_memory.clear();
			_map.clear();
		}

		ValueWrapper TryGet(const Key* key)
		{
			auto it = _map.find(*key);
			if (it != _map.end())
			{
				return it->second;
			}
			else
			{
				return ValueWrapper();
			}
		}

		ValueWrapper Get(const Key* key, const InitializationData* initData = nullptr)
		{
			ValueWrapper val = TryGet(key);
			if (!IsValidValueWrapper(&val))
			{
				val = CreateValue(key, initData);
				_map.emplace(*key, val);
			}
			return val;
		}

	protected:

		std::vector<Value> _memory;
		std::unordered_map<Key, ValueWrapper> _map;

		virtual Value* AllocateValue()
		{
			Value val;
			_memory.push_back(val);
			return &_memory.back();
		}

		virtual ValueWrapper CreateValue(const Key* key, const InitializationData* initData) = 0;
		virtual bool IsValidValueWrapper(const ValueWrapper* val) = 0;
	};
}