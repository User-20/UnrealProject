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

	/* ChatWindow를 보여줌 */
	void ShowChatWindow();

	/*
	 * ChatWindow가 없다면 생성한다  
	 *
	 * return	위젯이 생성된 경우	true
	 */
	bool TryCreateChatWidget();

	/*
	 * 문자열을 ChatWindow에 추가함 
	 *
	 * @param	ChatString	추가할 문자열
	 */
	void AddChatLine(const FText& ChatString);

	/* ChatWindow TextBox에 키보드 포커스 설정 */
	void ToggleChatWindow();

protected:
	class UChatWindow* ChatWindow;
};
