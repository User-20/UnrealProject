// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MultiRPGCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h "

#include "Public/MultiRPGGameInstance.h"
#include "Public/UI/WidgetManager.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

//////////////////////////////////////////////////////////////////////////
// AMultiRPGCharacter

AMultiRPGCharacter::AMultiRPGCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bReplicates = true;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	USkeletalMeshComponent* CharacterMesh = GetMesh();
	// ĳ���� � ���⸦ ���� ������ ����
	WeaponBack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponBack"));
	WeaponBack->SetupAttachment(CharacterMesh, TEXT("WeaponBack"));

	// ĳ���� �����տ� ���⸦ ���� ����
	WeaponRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponRight"));
	WeaponRight->SetupAttachment(CharacterMesh, TEXT("WeaponRight"));

	// ĳ���Ϳ� �ʿ��� ������ �ε���
	AssetLoaded();

	// ó������ ��� �������� �ʾ����Ƿ� �ʱ�ȭ�� ���ش�
	bIsRifleEquipped = false;

}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiRPGCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMultiRPGCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultiRPGCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMultiRPGCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMultiRPGCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMultiRPGCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMultiRPGCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMultiRPGCharacter::OnResetVR);

	PlayerInputComponent->BindAction("EquipRifle", IE_Pressed, this, &AMultiRPGCharacter::EquipRifle);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMultiRPGCharacter::Fire);
}

void AMultiRPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Controller�� AMultiRPGPlayerController ��ȯ�Ѵ�
	AMultiRPGPlayerController* PlayerController = Cast<AMultiRPGPlayerController>(Controller);
	if (PlayerController)
	{
		// WIdgetManager���� Hud CrossHair�� Setup�ϰ� addToViewport�� ������
		AWidgetManager* WidgetManager = PlayerController->GetWidgetManager();
		if (WidgetManager)
		{
			WidgetManager->OpenWidget<class UHUDCrosshair>(WidgetName::CROSS_HAIR, EWidgetDepth::OVERLAY);
		}
	}
}

void AMultiRPGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiRPGCharacter, fMoveForward);
	DOREPLIFETIME(AMultiRPGCharacter, fMoveRight);
	DOREPLIFETIME(AMultiRPGCharacter, bIsRifleEquipped);
}

void AMultiRPGCharacter::ServerRPCSetMovement_Implementation(float Movement, bool isForward)
{
	if (isForward)
	{
		if (fMoveForward != Movement)
		{
			fMoveForward = Movement;
		}
	}
	else
	{
		if (fMoveRight != Movement)
		{
			fMoveRight = Movement;
		}
	}
}

bool AMultiRPGCharacter::ServerRPCSetMovement_Validate(float Movement, bool isForward)
{
	return true;
}

void AMultiRPGCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMultiRPGCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AMultiRPGCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AMultiRPGCharacter::AssetLoaded()
{
	// ���� ������ ��� �´�
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WeaponAsset(TEXT("/Game/Item/FPS_Weapon_Bundle/Weapons/Meshes/KA_Val/SM_KA_Val_Y"));
	if (WeaponAsset.Succeeded())
	{
		PlayerWeapon = WeaponAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FireAnimMontage(TEXT("/Game/Characters/Mannequin/Animations/Fire_Rifle_Hip_Montage"));
	if (FireAnimMontage.Succeeded())
	{
		FireAnim = FireAnimMontage.Object;
	}

}

void AMultiRPGCharacter::EquipRifle()
{
	ServerRPC_EquipRifle();

}

void AMultiRPGCharacter::ServerRPC_EquipRifle_Implementation()
{
	bIsRifleEquipped = !bIsRifleEquipped;
}

bool AMultiRPGCharacter::ServerRPC_EquipRifle_Validate()
{
	return true;
}

void AMultiRPGCharacter::OnRep_EquipRifle()
{
	if (bIsRifleEquipped)
	{
		// ��� �����ߴٸ� � �ִ� ���⸦ �����
		WeaponBack->SetStaticMesh(nullptr);
		// ��� �����տ� ������
		WeaponRight->SetStaticMesh(PlayerWeapon);
	}
	else
	{
		// ����� ���� ������ �ߴٸ� � ���⸦ ������
		WeaponBack->SetStaticMesh(PlayerWeapon);
		// ��� �����տ��� �����
		WeaponRight->SetStaticMesh(nullptr);
	}
}

void AMultiRPGCharacter::Fire()
{
	if (bIsRifleEquipped)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(FireAnim);

		// �������� Fire �ִϸ��̼��� play �ߴٰ� �˷���
		ServerRPC_FireAnimation();
	}
	
}

void AMultiRPGCharacter::ServerRPC_FireAnimation_Implementation()
{
	// �ڽ��� Fire �ִϸ��̼��� Play �ߴٰ� ��� Ŭ���̾�Ʈ�� ���� �˷���
	MulticastRPC_FireAnimation();
}

bool AMultiRPGCharacter::ServerRPC_FireAnimation_Validate()
{
	return true;
}

void AMultiRPGCharacter::MulticastRPC_FireAnimation_Implementation()
{
	if (!HasAuthority())
	{
		// ����Ʈ �÷��̾� �鿡�� �ִϸ��̼��� Play ��Ű��� �˷���
		if (Controller != GetWorld()->GetFirstPlayerController())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(FireAnim);
		}
	}
}

bool AMultiRPGCharacter::MulticastRPC_FireAnimation_Validate()
{
	return true;
}

void AMultiRPGCharacter::OnShoot()
{
	if (!HasAuthority())
	{
		UMultiRPGGameInstance* GameInstance = Cast<UMultiRPGGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			ParticleManager* _ParticleManager = GameInstance->GetParticleManager();
			if (_ParticleManager)
			{
				// �� �� ������ �Ҳ� ��ƼŬ�� �ѱ��� ������ Play ��Ŵ
				const FString ShootParticlePath = TEXT("WeaponEffects/P_AssaultRifle_MF");
				const FName SocketName = TEXT("Muzzle");

				_ParticleManager->PlayParticle(ShootParticlePath, WeaponRight, SocketName, EAttachLocation::SnapToTarget);

				// �� ��� ���带 Play ��
				SoundManager* _SoundManager = GameInstance->GetSoundManager();
				if (_SoundManager)
				{
					const FString SoundName = TEXT("rifle_sound");
					_SoundManager->PlaySound(GetWorld(), SoundName, GetActorLocation());
				}

				// ���� �� �ڽ��� Ŭ���̾�Ʈ�� ����
				if (Controller == GetWorld()->GetFirstPlayerController())
				{
					// �� ��� �������� LineTrace
					// SprintArm�� ��ġ�� Start�������� ��
					const FVector Start = CameraBoom->GetComponentLocation();

					// ī�޶� �ٶ󺸴� �������� Bullet�� ���ư� �Ÿ�(10000.0f)�� ���� ���� End ����
					const float BulletDistance = 10000.0f;
					const FVector End = Start + (FollowCamera->GetForwardVector() * BulletDistance);

					FHitResult HitData(ForceInit);
					// TraceComplex �ɼ��� Ű��, �� �ڽ��� LineTrace�浹���� ������
					FCollisionQueryParams TraceParameters(FName(TEXT("")), true, GetOwner());
					// �浹�� üũ �ؾ��� Object Type
					FCollisionObjectQueryParams CollisionParams;
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Vehicle);
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Destructible);

					GetWorld()->LineTraceSingleByObjectType(HitData, Start, End, CollisionParams, TraceParameters);
					//DrawDebugLine(GetWorld(), Start, End, FColor::Green, true);

					// LineTrace�� �浹�� ��ü�� �ִٸ�
					AActor* ActorHit = HitData.GetActor();
					if (ActorHit)
					{
						// ��ƼŬ (���� ����Ʈ)�� ���� �Ĺ����� ������ �ʴ� ������ �־�
						// �浹�� ��ü�� Normal �������� 20.0f ���� ������ Offset���� ��
						const FVector Offset = HitData.Normal * 20.0f;
						const FVector ImpactPoint = HitData.ImpactPoint + Offset;
						// �浹�� ��ġ�� ��ƼŬ�� Play ��
						const FString HitParticlePath = TEXT("WeaponEffects/P_AssaultRifle_IH");
						_ParticleManager->PlayParticle(GetWorld(), HitParticlePath, ImpactPoint);

						// �ǰݵ� ��ġ�� ������ �˷���
						SeverRPC_ShowAttackEffect(HitParticlePath, ImpactPoint);
					}
				}
			}
		}
	}
}

void AMultiRPGCharacter::SeverRPC_ShowAttackEffect_Implementation(const FString& ParticlePath, const FVector & SpawnLocation)
{
	MulticastRPC_ShowAttackEffect(ParticlePath, SpawnLocation);
}

bool AMultiRPGCharacter::SeverRPC_ShowAttackEffect_Validate(const FString& ParticlePath, const FVector & SpawnLocation)
{
	return true;
}

void AMultiRPGCharacter::MulticastRPC_ShowAttackEffect_Implementation(const FString& ParticlePath, const FVector& SpawnLocation)
{
	if (!HasAuthority())
	{
		// ����Ʈ �÷��̾���
		if (Controller != GetWorld()->GetFirstPlayerController())
		{
			UMultiRPGGameInstance* GameInstance = Cast<UMultiRPGGameInstance>(GetGameInstance());
			if (GameInstance)
			{
				ParticleManager* _ParticleManager = GameInstance->GetParticleManager();
				if (_ParticleManager)
				{
					// ��ƼŬ�� ���� ��Ų��
					_ParticleManager->PlayParticle(GetWorld(), ParticlePath, SpawnLocation);
				}
			}
		}
	}
}

bool AMultiRPGCharacter::MulticastRPC_ShowAttackEffect_Validate(const FString& ParticlePath, const FVector& SpawnLocation)
{
	return true;
}

void AMultiRPGCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMultiRPGCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMultiRPGCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}

	ServerRPCSetMovement(Value, true);
}

void AMultiRPGCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement in that direction
		AddMovementInput(Direction, Value);
	}

	ServerRPCSetMovement(Value, false);
}
