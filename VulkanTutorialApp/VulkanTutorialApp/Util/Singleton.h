#pragma once

namespace Util
{
	template <class T>
	class Singleton
	{
	public:

		static T* GetInstance()
		{
			if (_instance == nullptr)
			{
				CreateInstance();
			}

			return _instance;
		}

		static void CreateInstance()
		{
			JE_Assert(_instance == nullptr);
			_instance = new T();
		}

		static void DestroyInstance()
		{
			JE_Assert(_instance != nullptr);
			delete _instance;
			_instance = nullptr;
		}

	private:

		static T * _instance;

	};

	template <class T> T* Singleton<T>::_instance = nullptr;
}