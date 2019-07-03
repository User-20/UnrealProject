// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"

class MULTIRPG_API ParticleManager
{

public:
	ParticleManager();
	~ParticleManager();

	/* ��ƼŬ�� Map�� ��Ƶ�
	 *
	 * @Param		FileName		��ƼŬ������ ���ϸ� (Effect ���� ������ �����ϸ� ������ ����)
	 */
	void AddParticle(const FString& FileName);

	/**
	 * ��ƼŬ�� ��ȯ�� 
	 * 
	 * @Param		FileName		��ƼŬ������ ���ϸ� (Effect ���� ������ �����ϸ� ������ ����)
	 */
	UParticleSystem* GetParticleEffect(const FString& FileName);

	/* ������ ������Ʈ�� ��ƼŬ�� ������ ���� �� ���� ��Ŵ (SpawnEmitterAttached ȣ����)
	 *
	 * @Param		Name					�ҷ��� ��ƼŬ�� ���� Key (Effect ���� ������ �����ϸ� ������ ����)
	 * @Param		AttachToComponent		��ƼŬ�� ���� ������Ʈ (ex. �޽�)
	 * @Param		AttachPointName			static mesh ���� �̸�
	 * @Param		LocationType			��ġ�� ���� ���� (��ġ�� ��� ���������� ���� ���� ��ġ���� ����)
	 * @Param		Location				��ƼŬ�� ��ġ�� (offset)
	 * @Param		Rotation				��ƼŬ�� ȸ���� (offset)
	 * @Param		Scale					��ƼŬ�� ũ�� (offset)
	 */
	void PlayParticle(const FString& Name, class USceneComponent* AttachToComponent, FName AttachPointName = NAME_None,
						EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset,
						FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f));

	/* ������ ��ġ�� ��ƼŬ�� ���� ��Ŵ (SpawnEmitterAtLocation)
	*
	* @Param		Name					�ҷ��� ��ƼŬ�� ���� Key (Effect ���� ������ �����ϸ� ������ ����)
	* @Param		Location				��ƼŬ�� ��ġ�� (offset)
	* @Param		Rotation				��ƼŬ�� ȸ���� (offset)
	* @Param		Scale					��ƼŬ�� ũ�� (offset)
	*/
	void PlayParticle(const UObject* WorldContext, const FString& Name, FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f));

private:
	TMap<FString, UParticleSystem*> Particles;
};
