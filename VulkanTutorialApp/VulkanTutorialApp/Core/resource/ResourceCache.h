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
			for (auto& pair : _map)
			{
				pair.second->Cleanup();
				delete pair.second;
			}
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
				Value* valPtr = new Value();
				valPtr->Load(key, loadOpts);
				_map.emplace(key, valPtr);
				return valPtr;
			}
		}

	private:

		std::unordered_map<std::string, Value*> _map;
	};
}

