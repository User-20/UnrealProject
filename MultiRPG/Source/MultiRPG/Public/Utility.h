#pragma once

#include "CoreMinimal.h"

template <typename T>
FORCEINLINE T* LoadObjectFromPath(const FString& Path)
{
	if (Path.IsEmpty())
	{
		return nullptr;
	}
	return Cast<T>(StaticLoadObject(T::StaticClass(), NULL, *Path));
}