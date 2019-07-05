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
	// 캐릭터 등에 무기를 붙힐 소켓을 만듦
	WeaponBack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponBack"));
	WeaponBack->SetupAttachment(CharacterMesh, TEXT("WeaponBack"));

	// 캐릭터 오른손에 무기를 붙힐 소켓
	WeaponRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponRight"));
	WeaponRight->SetupAttachment(CharacterMesh, TEXT("WeaponRight"));

	// 캐릭터에 필요한 에셋을 로드함
	AssetLoaded();

	// 처음에는 장비를 장착하지 않았으므로 초기화를 해준다
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
	
	// Controller를 AMultiRPGPlayerController 변환한다
	AMultiRPGPlayerController* PlayerController = Cast<AMultiRPGPlayerController>(Controller);
	if (PlayerController)
	{
		// WIdgetManager에서 Hud CrossHair를 Setup하고 addToViewport를 실행함
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
	// 무기 에셋을 들고 온다
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
		// 장비를 장착했다면 등에 있는 무기를 지운다
		WeaponBack->SetStaticMesh(nullptr);
		// 장비를 오른손에 붙힌다
		WeaponRight->SetStaticMesh(PlayerWeapon);
	}
	else
	{
		// 장비의 장착 해제를 했다면 등에 무기를 붙힌다
		WeaponBack->SetStaticMesh(PlayerWeapon);
		// 장비를 오른손에서 지운다
		WeaponRight->SetStaticMesh(nullptr);
	}
}

void AMultiRPGCharacter::Fire()
{
	if (bIsRifleEquipped)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(FireAnim);

		// 서버에게 Fire 애니메이션을 play 했다고 알려줌
		ServerRPC_FireAnimation();
	}
	
}

void AMultiRPGCharacter::ServerRPC_FireAnimation_Implementation()
{
	// 자신이 Fire 애니메이션을 Play 했다고 모든 클라이언트들 에게 알려줌
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
		// 리모트 플레이어 들에게 애니메이션을 Play 시키라고 알려줌
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
				// 총 쏠때 나오는 불꽃 파티클을 총구에 붙히고 Play 시킴
				const FString ShootParticlePath = TEXT("WeaponEffects/P_AssaultRifle_MF");
				const FName SocketName = TEXT("Muzzle");

				_ParticleManager->PlayParticle(ShootParticlePath, WeaponRight, SocketName, EAttachLocation::SnapToTarget);

				// 총 쏘는 사운드를 Play 함
				SoundManager* _SoundManager = GameInstance->GetSoundManager();
				if (_SoundManager)
				{
					const FString SoundName = TEXT("rifle_sound");
					_SoundManager->PlaySound(GetWorld(), SoundName, GetActorLocation());
				}

				// 총을 쏜 자신의 클라이언트만 실행
				if (Controller == GetWorld()->GetFirstPlayerController())
				{
					// 총 쏘는 방향으로 LineTrace
					// SprintArm의 위치를 Start지점으로 함
					const FVector Start = CameraBoom->GetComponentLocation();

					// 카메라가 바라보는 방향으로 Bullet이 날아갈 거리(10000.0f)를 곱한 값이 End 지점
					const float BulletDistance = 10000.0f;
					const FVector End = Start + (FollowCamera->GetForwardVector() * BulletDistance);

					FHitResult HitData(ForceInit);
					// TraceComplex 옵션을 키고, 나 자신은 LineTrace충돌에서 무시함
					FCollisionQueryParams TraceParameters(FName(TEXT("")), true, GetOwner());
					// 충돌을 체크 해야할 Object Type
					FCollisionObjectQueryParams CollisionParams;
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Vehicle);
					CollisionParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Destructible);

					GetWorld()->LineTraceSingleByObjectType(HitData, Start, End, CollisionParams, TraceParameters);
					//DrawDebugLine(GetWorld(), Start, End, FColor::Green, true);

					// LineTrace에 충돌된 물체가 있다면
					AActor* ActorHit = HitData.GetActor();
					if (ActorHit)
					{
						// 파티클 (연기 이펙트)가 벽에 파뭍혀서 보이지 않는 현상이 있어
						// 충돌된 물체의 Normal 방향으로 20.0f 정도 떨어진 Offset값을 줌
						const FVector Offset = HitData.Normal * 20.0f;
						const FVector ImpactPoint = HitData.ImpactPoint + Offset;
						// 충돌된 위치에 파티클을 Play 함
						const FString HitParticlePath = TEXT("WeaponEffects/P_AssaultRifle_IH");
						_ParticleManager->PlayParticle(GetWorld(), HitParticlePath, ImpactPoint);

						// 피격된 위치를 서버에 알려줌
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
		// 리모트 플레이어라면
		if (Controller != GetWorld()->GetFirstPlayerController())
		{
			UMultiRPGGameInstance* GameInstance = Cast<UMultiRPGGameInstance>(GetGameInstance());
			if (GameInstance)
			{
				ParticleManager* _ParticleManager = GameInstance->GetParticleManager();
				if (_ParticleManager)
				{
					// 파티클을 실행 시킨다
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
