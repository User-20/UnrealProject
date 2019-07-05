// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiRPGCharacter.generated.h"

UCLASS(config=Game)
class AMultiRPGCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AMultiRPGCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/* 캐릭터에 필요한 에셋을 로드함 */
	void AssetLoaded();

	/* 무기 장착 Input에 불리는 함수 */
	void EquipRifle();

	UFUNCTION(reliable, Server, WithValidation)
	void ServerRPC_EquipRifle();
	virtual void ServerRPC_EquipRifle_Implementation();
	virtual bool ServerRPC_EquipRifle_Validate();

	UFUNCTION()
	virtual void OnRep_EquipRifle();

	/* 무기 발사 Input 함수 */
	void Fire();

	/* 총쏘는 애니메이션 서버에서 실행 (멀티캐스트를 사용하기 위함) */
	UFUNCTION(Unreliable, Server, WithValidation)
	void ServerRPC_FireAnimation();
	virtual void ServerRPC_FireAnimation_Implementation();
	virtual bool ServerRPC_FireAnimation_Validate();

	/* 총쏘는 애니메이션 멀티캐스트(다른 클라이언트에서 실행) */
	UFUNCTION(Unreliable, NetMulticast, WithValidation)
	void MulticastRPC_FireAnimation();
	virtual void MulticastRPC_FireAnimation_Implementation();
	virtual bool MulticastRPC_FireAnimation_Validate();

	/* 플레이어 공격 애니메이션 몽타주에서 호출됨 */
	UFUNCTION(BlueprintCallable, Category = "Attack", meta = (AllowProtectedAccess = "true"))
	void OnShoot();

	/* LineTrace이후 피격 지점을 서버에 알려줌
	 *
	 * @Param		SpawnLocation		피격된 위치
	 */
	UFUNCTION(Unreliable, Server, WithValidation)
	void SeverRPC_ShowAttackEffect(const FString& ParticlePath, const FVector& SpawnLocation);
	void SeverRPC_ShowAttackEffect_Implementation(const FString& ParticlePath, const FVector& SpawnLocation);
	bool SeverRPC_ShowAttackEffect_Validate(const FString& ParticlePath, const FVector& SpawnLocation);

	UFUNCTION(Unreliable, NetMulticast, WithValidation)
	void MulticastRPC_ShowAttackEffect(const FString& ParticlePath, const FVector& SpawnLocation);
	virtual void MulticastRPC_ShowAttackEffect_Implementation(const FString& ParticlePath, const FVector& SpawnLocation);
	virtual bool MulticastRPC_ShowAttackEffect_Validate(const FString& ParticlePath, const FVector& SpawnLocation);

	/** 이동 애니메이션에 사용 하는 (fMoveFoward, fMoveRight) 를 서버에서 실행 시킴 (서버에서 실행 시킬 몸체)
	 * 
	 * @Param		Movement		Input이 들어왔을 때 매개변수로 들어오는 ScaleValue
	 * @Param		isForward		ture는 전진 및 후진, false 는 좌 우
	 */
	UFUNCTION(BlueprintCallable, Unreliable, Server, WithValidation, meta = (AllowProtectedAccess = "true"))
	void ServerRPCSetMovement(float Movement, bool isForward = true);
	virtual void ServerRPCSetMovement_Implementation(float MoveFoward, bool isForward = true);
	virtual bool ServerRPCSetMovement_Validate(float MoveFoward, bool isForward = true);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

private:
	/* 등에 무기를 붙힐 소켓 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponBack;

	/* 손에 무기를 붙힐 소켓 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponRight;

	/* 캐릭터 애니메이션 앞뒤 움직임에 정보를 담고 있음 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float fMoveForward;

	/* 캐릭터 애니메이션 좌우 움직임에 정보를 담고 있음 */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float fMoveRight;

	/* 무기를 장착 했는지? */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Equip", ReplicatedUsing = OnRep_EquipRifle, meta = (AllowPrivateAccess = "true"))
	bool bIsRifleEquipped;

protected:
	class UStaticMesh* PlayerWeapon;
	class UAnimMontage* FireAnim;
};
