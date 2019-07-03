// Fill out your copyright notice in the Description page of Project Settings.


#include "ParticleManager.h"
#include "Public/Utility.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/Misc/Paths.h"

ParticleManager::ParticleManager()
{
}

ParticleManager::~ParticleManager()
{
	for (auto particle : Particles)
	{
		particle.Value = nullptr;
	}

	Particles.Empty();
}

void ParticleManager::AddParticle(const FString& FileName)
{
	if (FileName.IsEmpty())
	{
		return;
	}

	FString Path = TEXT("/Game/Effect/") + FileName;
	UParticleSystem* Target = LoadObjectFromPath<UParticleSystem>(Path);
	if (Target)
	{
		Particles.Emplace(FileName, Target);
	}
}


UParticleSystem * ParticleManager::GetParticleEffect(const FString& Name)
{
	if (Particles.Find(Name) == nullptr)
	{
		AddParticle(Name);
	}

	return Particles[Name];
}

void ParticleManager::PlayParticle(const FString& Name, USceneComponent * AttachToComponent, FName AttachPointName, EAttachLocation::Type LocationType, FVector Location, FRotator Rotation, FVector Scale)
{
	UParticleSystem* Effect = GetParticleEffect(Name);

	if (Effect)
	{
		UGameplayStatics::SpawnEmitterAttached(Effect, AttachToComponent, AttachPointName
			, Location, Rotation, EAttachLocation::SnapToTarget);
	}

}

void ParticleManager::PlayParticle(const UObject* WorldContext, const FString & Name, FVector Location, FRotator Rotation, FVector Scale)
{
	UParticleSystem* Effect = GetParticleEffect(Name);

	if (Effect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(WorldContext, Effect, Location, Rotation, Scale);
	}
}
