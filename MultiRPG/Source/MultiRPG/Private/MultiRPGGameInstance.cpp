// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiRPGGameInstance.h"

UMultiRPGGameInstance::UMultiRPGGameInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, _ParticleManager(nullptr)
	, _SoundManager(nullptr)
{

}

void UMultiRPGGameInstance::FinishDestroy()
{
	if (_ParticleManager)
	{
		delete _ParticleManager;
		_ParticleManager = nullptr;
	}

	if (_SoundManager)
	{
		delete _SoundManager;
		_SoundManager = nullptr;
	}

	Super::FinishDestroy();
}

ParticleManager * UMultiRPGGameInstance::GetParticleManager()
{
	if (_ParticleManager == nullptr)
	{
		_ParticleManager = new ParticleManager();
	}

	return _ParticleManager;
}

SoundManager * UMultiRPGGameInstance::GetSoundManager()
{
	if (_SoundManager == nullptr)
	{
		_SoundManager = new SoundManager();
	}

	return _SoundManager;
}
