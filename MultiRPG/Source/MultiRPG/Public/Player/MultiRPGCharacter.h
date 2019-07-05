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

	/* ĳ���Ϳ� �ʿ��� ������ �ε��� */
	void AssetLoaded();

	/* ���� ���� Input�� �Ҹ��� �Լ� */
	void EquipRifle();

	UFUNCTION(reliable, Server, WithValidation)
	void ServerRPC_EquipRifle();
	virtual void ServerRPC_EquipRifle_Implementation();
	virtual bool ServerRPC_EquipRifle_Validate();

	UFUNCTION()
	virtual void OnRep_EquipRifle();

	/* ���� �߻� Input �Լ� */
	void Fire();

	/* �ѽ�� �ִϸ��̼� �������� ���� (��Ƽĳ��Ʈ�� ����ϱ� ����) */
	UFUNCTION(Unreliable, Server, WithValidation)
	void ServerRPC_FireAnimation();
	virtual void ServerRPC_FireAnimation_Implementation();
	virtual bool ServerRPC_FireAnimation_Validate();

	/* �ѽ�� �ִϸ��̼� ��Ƽĳ��Ʈ(�ٸ� Ŭ���̾�Ʈ���� ����) */
	UFUNCTION(Unreliable, NetMulticast, WithValidation)
	void MulticastRPC_FireAnimation();
	virtual void MulticastRPC_FireAnimation_Implementation();
	virtual bool MulticastRPC_FireAnimation_Validate();

	/* �÷��̾� ���� �ִϸ��̼� ��Ÿ�ֿ��� ȣ��� */
	UFUNCTION(BlueprintCallable, Category = "Attack", meta = (AllowProtectedAccess = "true"))
	void OnShoot();

	/* LineTrace���� �ǰ� ������ ������ �˷���
	 *
	 * @Param		SpawnLocation		�ǰݵ� ��ġ
	 */
	UFUNCTION(Unreliable, Server, WithValidation)
	void SeverRPC_ShowAttackEffect(const FString& ParticlePath, const FVector& SpawnLocation);
	void SeverRPC_ShowAttackEffect_Implementation(const FString& ParticlePath, const FVector& SpawnLocation);
	bool SeverRPC_ShowAttackEffect_Validate(const FString& ParticlePath, const FVector& SpawnLocation);

	UFUNCTION(Unreliable, NetMulticast, WithValidation)
	void MulticastRPC_ShowAttackEffect(const FString& ParticlePath, const FVector& SpawnLocation);
	virtual void MulticastRPC_ShowAttackEffect_Implementation(const FString& ParticlePath, const FVector& SpawnLocation);
	virtual bool MulticastRPC_ShowAttackEffect_Validate(const FString& ParticlePath, const FVector& SpawnLocation);

	/** �̵� �ִϸ��̼ǿ� ��� �ϴ� (fMoveFoward, fMoveRight) �� �������� ���� ��Ŵ (�������� ���� ��ų ��ü)
	 * 
	 * @Param		Movement		Input�� ������ �� �Ű������� ������ ScaleValue
	 * @Param		isForward		ture�� ���� �� ����, false �� �� ��
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
	/* � ���⸦ ���� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponBack;

	/* �տ� ���⸦ ���� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponRight;

	/* ĳ���� �ִϸ��̼� �յ� �����ӿ� ������ ��� ���� */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float fMoveForward;

	/* ĳ���� �ִϸ��̼� �¿� �����ӿ� ������ ��� ���� */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float fMoveRight;

	/* ���⸦ ���� �ߴ���? */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Equip", ReplicatedUsing = OnRep_EquipRifle, meta = (AllowPrivateAccess = "true"))
	bool bIsRifleEquipped;

protected:
	class UStaticMesh* PlayerWeapon;
	class UAnimMontage* FireAnim;
};
