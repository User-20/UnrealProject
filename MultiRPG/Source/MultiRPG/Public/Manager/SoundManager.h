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
	 * 사운드를 Map에 담아둠
	 *
	 * @Param		Name			Map에 사용될 Key (ex. 사운드 파일 이름)
	 */
	void AddSound(const FString& Name);

	/* 사운드를 실행 시킴
	 *
	 * @Param		WorldContext	월드에 대한 정보 (사운드를 실행 시킬 공간)
	 * @Param		Name			사운드를 실행시킬 Key (ex. 사운드 파일 이름)
	 * @Param		Location		사운드가 나오는 위치
	 */
	void PlaySound(const UObject* WorldContext, const FString& Name, const FVector& Location);

private:
	TMap<FString, USoundBase*> Sounds;
};
