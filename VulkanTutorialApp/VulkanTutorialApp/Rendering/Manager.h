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

		void ReinitializeValue(const Key* key)
		{
			ValueWrapper wrapper = TryGet(key);
			Value* value;
			if ((value = GetValueFromWrapper(wrapper)) != nullptr)
			{
				value->Reinitialize();
			}
		}

		// Cleanup and destroy all cached resources.
		virtual void Cleanup()
		{
			for (Value& value : _memory)
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
			if (GetValueFromWrapper(&val) == nullptr)
			{
				val = CreateValue(key, initData);
				_map.emplace(*key, val);
			}
			return val;
		}

		const Key* GetKey(const Value* value)
		{
			auto findResult = std::find_if(std::begin(_map), std::end(_map), [&](const std::pair<Key, ValueWrapper> &pair)
			{
				Value* val = GetValueFromWrapper(&pair.second);
				return val == value;
			});
			if (findResult != std::end(_map))
			{
				return &findResult->first;
			}
			else
			{
				return nullptr;
			}
		}

	protected:

		std::vector<Value> _memory;
		std::unordered_map<Key, ValueWrapper> _map;

		virtual Value* AllocateValue()
		{
			_memory.push_back(Value());
			return &_memory.back();
		}

		virtual ValueWrapper CreateValue(const Key* key, const InitializationData* initData) = 0;
		virtual Value* GetValueFromWrapper(const ValueWrapper* val) = 0;
	};
}