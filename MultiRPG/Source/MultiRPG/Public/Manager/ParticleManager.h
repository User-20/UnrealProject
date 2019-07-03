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

	/* 파티클을 Map에 담아둠
	 *
	 * @Param		FileName		파티클에셋의 파일명 (Effect 이후 폴더가 존재하면 폴더명 까지)
	 */
	void AddParticle(const FString& FileName);

	/**
	 * 파티클을 반환함 
	 * 
	 * @Param		FileName		파티클에셋의 파일명 (Effect 이후 폴더가 존재하면 폴더명 까지)
	 */
	UParticleSystem* GetParticleEffect(const FString& FileName);

	/* 지정된 컴포넌트에 파티클을 붙히고 월드 상에 실행 시킴 (SpawnEmitterAttached 호출함)
	 *
	 * @Param		Name					불러올 파티클에 대한 Key (Effect 이후 폴더가 존재하면 폴더명 까지)
	 * @Param		AttachToComponent		파티클을 붙힐 컴포넌트 (ex. 메쉬)
	 * @Param		AttachPointName			static mesh 소켓 이름
	 * @Param		LocationType			위치에 대한 유형 (위치가 상대 오프셋인지 절대 월드 위치인지 지정)
	 * @Param		Location				파티클의 위치값 (offset)
	 * @Param		Rotation				파티클의 회전값 (offset)
	 * @Param		Scale					파티클의 크기 (offset)
	 */
	void PlayParticle(const FString& Name, class USceneComponent* AttachToComponent, FName AttachPointName = NAME_None,
						EAttachLocation::Type LocationType = EAttachLocation::KeepRelativeOffset,
						FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f));

	/* 지정된 위치에 파티클을 실행 시킴 (SpawnEmitterAtLocation)
	*
	* @Param		Name					불러올 파티클에 대한 Key (Effect 이후 폴더가 존재하면 폴더명 까지)
	* @Param		Location				파티클의 위치값 (offset)
	* @Param		Rotation				파티클의 회전값 (offset)
	* @Param		Scale					파티클의 크기 (offset)
	*/
	void PlayParticle(const UObject* WorldContext, const FString& Name, FVector Location = FVector(ForceInit), FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f));

private:
	TMap<FString, UParticleSystem*> Particles;
};
