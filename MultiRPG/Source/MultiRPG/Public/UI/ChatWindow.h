// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Public/UI/MultiRPGWidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "ChatWindow.generated.h"

class UScrollBox;
class UEditableTextBox;
class UVerticalBox;

UCLASS()
class MULTIRPG_API UChatWindow : public UMultiRPGWidgetBase
{
	GENERATED_BODY()

public:
	UChatWindow(const FObjectInitializer& ObjectInitializer);

public:
	virtual void NativeConstruct() override;

	// 블루프린트 UI 변수 (채팅 기록에 대한 변수)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UScrollBox* ChatHistory;

	// 블루프린트 UI 변수 (입력 받는 부분)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UEditableTextBox* ChatEntry;

public:
	void Setup(APlayerController * PlayerController) override;

	/* 텍스트 커밋될 때 호출 됨
	 *
	 * @param		InText			커밋할 텍스트
	 * @param		InCommitInfo	커밋에 대한 유형 (ex. 엔터키로 입력받음, 포커스가 바뀜 등)
	 */
	UFUNCTION(BlueprintCallable)
	void OnChatTextCommitted(const FText& InText, ETextCommit::Type InCommitInfo);

	/* 채팅 기록을 추가 한다 */
	void AddChatLine(const FText& ChatString);

	
};
