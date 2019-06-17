// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiRPGWidgetBase.h"

UMultiRPGWidgetBase::UMultiRPGWidgetBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}


TWeakObjectPtr<AMultiRPGPlayerController> UMultiRPGWidgetBase::GetPlayerController() const
{
	
	if (OwnerController != nullptr)
	{
		return OwnerController;
	}
	return nullptr;
}

void UMultiRPGWidgetBase::Setup(APlayerController * PlayerController)
{
	SetOwningPlayer(PlayerController);
	OwnerController = Cast<AMultiRPGPlayerController>(PlayerController);
}
