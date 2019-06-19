// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UMG/Public/Components/SlateWrapperTypes.h"
#include "WidgetManager.generated.h"

/**
 * 
 */
UCLASS()
class MULTIRPG_API AWidgetManager : public AHUD
{
	GENERATED_BODY()

public:
	AWidgetManager(const FObjectInitializer& ObjectInitializer);

public:
	virtual void PostInitializeComponents() override;

	/* Set chat window visibility
	 * 
	 * @param	RequiredVisibility	The required visibility
	 */
	void SetChatVisibilty(const ESlateVisibility RequiredVisibility);

	/* ChatWindow�� ������ */
	void ShowChatWindow();

	/*
	 * ChatWindow�� ���ٸ� �����Ѵ�  
	 *
	 * return	������ ������ ���	true
	 */
	bool TryCreateChatWidget();

	/*
	 * ���ڿ��� ChatWindow�� �߰��� 
	 *
	 * @param	ChatString	�߰��� ���ڿ�
	 */
	void AddChatLine(const FText& ChatString);

	/* ChatWindow TextBox�� Ű���� ��Ŀ�� ���� */
	void ToggleChatWindow();

protected:
	class UChatWindow* ChatWindow;
};
