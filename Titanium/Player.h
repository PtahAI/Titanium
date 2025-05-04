#pragma once

#include "Framework.h"

namespace Player
{
	static void* (*ApplyCharacterCustomization)(void*, void*) = decltype(ApplyCharacterCustomization)(Helpers::GetOffset(0x146b740));

	void InitHooks()
	{
		
	}
}