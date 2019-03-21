#pragma once

namespace Util
{
	template 
	<
		typename EnclosedType, 
		typename ContextType = Util::NullType, 
		void(ContextType::*SetFuncAfter)() = nullptr,
		void(ContextType::*SetFuncBefore)(const EnclosedType& newObject, const EnclosedType& oldObject) = nullptr
	>
	class Property
	{
	public:

		Property()
		{
		}

		Property(const EnclosedType& object)
			: _object(object)
		{
		}

		Property(const Property& copy) = default;
		Property& operator=(const Property& copy) = default;
		~Property() 
		{ 
		}

		EnclosedType& Get() { return _object; }
		const EnclosedType& Get() const { return _object; }

		void Set(const EnclosedType& newObject)
		{
			static ContextType* context = nullptr;	// TODO

			if (context)
			{
				if (SetFuncBefore)
				{
					(context->*SetFuncBefore)(newObject, _object);
				}

				_object = newObject;

				if (SetFuncAfter)
				{
					(context->*SetFuncAfter)();
				}
			}
			else
			{
				_object = newObject;
			}
		}

	private:

		EnclosedType _object;

	};
}