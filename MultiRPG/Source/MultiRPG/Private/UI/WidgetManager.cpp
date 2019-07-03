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

	// WidgetDefinitions.xml �� ���
	FString filePath = (TEXT("Source/MultiRPG/Private/UI/WidgetDefinitions.xml"));
	// Project�� �����ϴ� ��θ� ���� ��η� �ٲ۴�
	FString FullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FullPath += filePath;

	// WidgetDefinitions.xml ������ Load��
	FXmlFile Difinitions(FullPath);

	const FXmlNode* node = Difinitions.GetRootNode();
	// Root Node�� �ڽĵ��� �����´�
	TArray<FXmlNode*> child = node->GetChildrenNodes();
	int childLength = child.Num();

	for (int i = 0; i < childLength; i++)
	{
		// �Ӽ����� Key�� �����´�
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