// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class USoundBase;

class MULTIRPG_API SoundManager
{
public:
	SoundManager();
	~SoundManager();

	/*
	 * ���带 Map�� ��Ƶ�
	 *
	 * @Param		Name			Map�� ���� Key (ex. ���� ���� �̸�)
	 */
	void AddSound(const FString& Name);

	/* ���带 ���� ��Ŵ
	 *
	 * @Param		WorldContext	���忡 ���� ���� (���带 ���� ��ų ����)
	 * @Param		Name			���带 �����ų Key (ex. ���� ���� �̸�)
	 * @Param		Location		���尡 ������ ��ġ
	 */
	void PlaySound(const UObject* WorldContext, const FString& Name, const FVector& Location);

private:
	TMap<FString, USoundBase*> Sounds;
};
