// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManager.h"
#include "Public/Utility.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
	for (auto sound : Sounds)
	{
		sound.Value = nullptr;
	}

	Sounds.Empty();
}

void SoundManager::AddSound(const FString & Name)
{
	if (Name.IsEmpty())
	{
		return;
	}

	FString Path = TEXT("/Game/Sounds/") + Name;
	
	USoundBase* Target = LoadObjectFromPath<USoundBase>(Path);
	if (Target)
	{
		Sounds.Emplace(Name, Target);
	}
}

void SoundManager::PlaySound(const UObject* WorldContext, const FString & Name, const FVector& Location)
{
	if (Name.IsEmpty())
	{
		return;
	}

	if (Sounds.Find(Name) == nullptr)
	{
		AddSound(Name);
	}
	
	UGameplayStatics::PlaySoundAtLocation(WorldContext, Sounds[Name], Location);
}
