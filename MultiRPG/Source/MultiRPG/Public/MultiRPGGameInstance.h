// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Public/Manager/ParticleManager.h"
#include "Public/Manager/SoundManager.h"
#include "MultiRPGGameInstance.generated.h"

UCLASS()
class MULTIRPG_API UMultiRPGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMultiRPGGameInstance(const FObjectInitializer& ObjectInitializer);
	
	virtual void FinishDestroy() override;

	/* ��ƼŬ �ý��� �����ϴ� Ŭ������ ��ȯ�� */
	ParticleManager* GetParticleManager();

	/* ���� �����ϴ� Ŭ������ ��ȯ�� */
	SoundManager* GetSoundManager();

private:
	ParticleManager* _ParticleManager;
	SoundManager* _SoundManager;
	
};
