// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiRPGPlayerController.h"
#include "Public/UI/WidgetManager.h"
#include "Public/Online/MultiRPGGameMode.h"
#include "Runtime/Engine/Classes/Engine/World.h"

AMultiRPGPlayerController::AMultiRPGPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void AMultiRPGPlayerController::BeginPlay()
{
	Super::BeginPlay();
	OnChatWindow();
}

void AMultiRPGPlayerController::SendText(const FString & Msg)
{
	ServerRPCSendText(Msg);
}

void AMultiRPGPlayerController::ServerRPCSendText_Implementation(const FString& Msg)
{
	GetWorld()->GetAuthGameMode<AMultiRPGGameMode>()->Broadcast(this, Msg, *this->GetName());
}

bool AMultiRPGPlayerController::ServerRPCSendText_Validate(const FString& Msg)
{
	return true;
}

void AMultiRPGPlayerController::ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime )
{
	AWidgetManager* WidgetManager = GetWidgetManager();
	if (WidgetManager)
	{
		if (SenderPlayerState != PlayerState)
		{
			WidgetManager->AddChatLine(FText::FromString(S));
		}
	}
}

AWidgetManager * AMultiRPGPlayerController::GetWidgetManager() const
{
	return Cast<AWidgetManager>(GetHUD());
}

void AMultiRPGPlayerController::OnChatWindow()
{
	AWidgetManager* WidgetManager = GetWidgetManager();
	if (WidgetManager)
	{
		WidgetManager->ShowChatWindow();
	}
}
