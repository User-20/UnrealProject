// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiRPGGameMode.generated.h"

class AMultiRPGPlayerController;

UCLASS()
class MULTIRPG_API AMultiRPGGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AMultiRPGGameMode(const FObjectInitializer& ObjectInitializer);
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;

private:
	TArray<AMultiRPGPlayerController*> AlivePCs;
};
