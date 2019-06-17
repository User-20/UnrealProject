// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiRPGGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "MultiRPGPlayerController.h"
#include "Public/UI/WidgetManager.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerState.h"
#include "Runtime/Engine/Classes/GameFramework/GameState.h"
#include "Runtime/Engine/Classes/GameFramework/SpectatorPawn.h"

AMultiRPGGameMode::AMultiRPGGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = AMultiRPGPlayerController::StaticClass();
	GameStateClass = AGameState::StaticClass();
	SpectatorClass = ASpectatorPawn::StaticClass();
	PlayerStateClass = APlayerState::StaticClass();
	HUDClass = AWidgetManager::StaticClass();

}

void AMultiRPGGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	// Login�� �÷��̾ �ִٸ�
	AMultiRPGPlayerController* PC = Cast< AMultiRPGPlayerController>(NewPlayer);
	if (PC)
	{
		// �迭�� ��´�.
		AlivePCs.Add(PC);
		
	}
}

void AMultiRPGGameMode::Logout(AController * Exiting)
{
	Super::Logout(Exiting);
	// Logout �÷��̾ �ִٸ�
	AMultiRPGPlayerController* PC = Cast< AMultiRPGPlayerController>(Exiting);
	if (PC)
	{
		// �迭���� �����
		AlivePCs.Remove(PC);
	}
}
