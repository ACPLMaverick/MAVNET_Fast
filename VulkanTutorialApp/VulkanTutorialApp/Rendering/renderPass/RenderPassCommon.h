#pragma once
namespace Rendering
{
	namespace RenderPassCommon
	{
		// TODO: This is a list of all supported render passes in the game engine.
		JE_EnumBegin(Id)
			Tutorial
			, Custom		// This is for functionality of adding custom render passes later on.
		JE_EnumEnd()
	}
}