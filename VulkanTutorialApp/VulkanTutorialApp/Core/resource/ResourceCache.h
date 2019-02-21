#pragma once

namespace Core
{
	template <class Value, class LoadOptions>
	class ResourceCache
	{
	public:
		ResourceCache() { }
		~ResourceCache() { }

		void Initialize() 
		{
		}

		void Cleanup()
		{
			for (Value& value : _array)
			{
				value.Cleanup();
			}
			_array.clear();
			_map.clear();
		}

		Value* Get(const std::string& key, const LoadOptions* loadOpts = nullptr)
		{
			auto findRes = _map.find(key);
			if (findRes != std::end(_map))
			{
				return findRes->second;
			}
			else
			{
				_array.push_back(Value());
				Value* valPtr = &(_array.back());
				valPtr->Load(key, loadOpts);
				_map.emplace(key, valPtr);
				return valPtr;
			}
		}

	private:

		std::vector<Value> _array;
		std::unordered_map<std::string, Value*> _map;
	};
}

