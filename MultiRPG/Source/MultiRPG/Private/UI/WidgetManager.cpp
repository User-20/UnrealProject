// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetManager.h"
#include "Public/UI/ChatWindow.h"
#include "Public/Player/MultiRPGPlayerController.h"
#include "Runtime/XmlParser/Public/XmlFile.h"
#include "Runtime/Core/Public/Misc/Paths.h"

AWidgetManager::AWidgetManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void AWidgetManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PlayerOwner = Cast<AMultiRPGPlayerController>(GetOwner());

	// WidgetDefinitions.xml 의 경로
	FString filePath = (TEXT("Source/MultiRPG/Private/UI/WidgetDefinitions.xml"));
	// Project가 존재하는 경로를 절대 경로로 바꾼다
	FString FullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FullPath += filePath;

	// WidgetDefinitions.xml 파일을 Load함
	FXmlFile Difinitions(FullPath);

	const FXmlNode* node = Difinitions.GetRootNode();
	// Root Node의 자식들을 가져온다
	TArray<FXmlNode*> child = node->GetChildrenNodes();
	int childLength = child.Num();

	for (int i = 0; i < childLength; i++)
	{
		// 속성값을 Key로 가져온다
		FString WidgetName = child[i]->GetAttribute(TEXT("Name"));
		FString WidgetPath = child[i]->GetAttribute(TEXT("Path"));
		
		if (!WidgetName.IsEmpty() && !WidgetPath.IsEmpty())
		{
			WidgetDefinitions.Emplace(WidgetName, WidgetPath);
		}
	}
	
}

void AWidgetManager::AddChatLine(const FText& ChatString)
{
	if (IsValidWidget(WidgetName::CHAT_WINDOW))
	{
		UChatWindow* ChatWindow = Cast<UChatWindow>(HUDLoadedMap[WidgetName::CHAT_WINDOW]);
		ChatWindow->AddChatLine(ChatString);
	}
}

void AWidgetManager::ToggleChatWindow()
{
	if (IsValidWidget(WidgetName::CHAT_WINDOW))
	{
		UChatWindow* ChatWindow = Cast<UChatWindow>(HUDLoadedMap[WidgetName::CHAT_WINDOW]);
		ChatWindow->ToggleTextBoxKeyboardFocus();
	}
}

bool AWidgetManager::IsValidWidget(const FString& WidgetName)
{
	if (HUDLoadedMap.Find(WidgetName))
	{
		return true;
	}

	return false;
}