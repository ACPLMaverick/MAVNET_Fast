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

		void Set(ContextType* context, const EnclosedType& newObject)
		{
			JE_Assert(context);

			if (_object == newObject)
			{
				return;
			}

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

	private:

		EnclosedType _object;

	};

#define JE_SetProperty(obj, propName, propValue) ((obj).propName.Set(&(obj), (propValue)))
#define JE_SetPropertyPtr(objPtr, propName, propValue) ((objPtr)->propName.Set((objPtr), (propValue)))
}