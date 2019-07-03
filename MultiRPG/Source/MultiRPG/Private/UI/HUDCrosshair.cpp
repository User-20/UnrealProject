// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDCrosshair.h"

UHUDCrosshair::UHUDCrosshair(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void UHUDCrosshair::Setup(APlayerController * PlayerController)
{
	Super::Setup(PlayerController);

	this->AddToViewport();
}
