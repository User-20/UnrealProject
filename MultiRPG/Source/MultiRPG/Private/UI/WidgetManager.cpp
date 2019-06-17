// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetManager.h"
#include "Public/UI/ChatWindow.h"
#include "Public/Player/MultiRPGPlayerController.h"

AWidgetManager::AWidgetManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AWidgetManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PlayerOwner = Cast<AMultiRPGPlayerController>(GetOwner());
}

void AWidgetManager::SetChatVisibilty(const ESlateVisibility RequiredVisibility)
{
	if (ChatWindow->GetVisibility() != RequiredVisibility)
	{
		ChatWindow->SetVisibility(RequiredVisibility);
	}
}

void AWidgetManager::ShowChatWindow()
{
	TryCreateChatWidget();

	if (ChatWindow)
	{
		ChatWindow->SetVisibility(ESlateVisibility::Visible);
	}	
}

void AWidgetManager::AddChatLine(const FText& ChatString)
{
	TryCreateChatWidget();

	if (ChatWindow)
	{
		ChatWindow->AddChatLine(ChatString);
	}
}

bool AWidgetManager::TryCreateChatWidget()
{
	bool bIsCreated = false;
	AMultiRPGPlayerController* PC = Cast<AMultiRPGPlayerController>(PlayerOwner);
	if (PC)
	{
		if (!ChatWindow)
		{
			bIsCreated = true;
			FStringClassReference WidgetClassRef(TEXT("/Game/UMG/Chat/WidgetBP_ChatWindow.WidgetBP_ChatWindow_C"));
			UClass * WidgetClass = WidgetClassRef.TryLoadClass <UChatWindow>();

			if (WidgetClass)
			{
				ChatWindow = CreateWidget<UChatWindow>(GetWorld(), WidgetClass);
				if (ChatWindow)
				{
					ChatWindow->Setup(PC);
				}
			}
		}
	}

	return bIsCreated;
}

