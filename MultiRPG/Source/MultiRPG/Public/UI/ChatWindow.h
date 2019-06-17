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

	// �������Ʈ UI ���� (ä�� ��Ͽ� ���� ����)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UScrollBox* ChatHistory;

	// �������Ʈ UI ���� (�Է� �޴� �κ�)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UEditableTextBox* ChatEntry;

public:
	void Setup(APlayerController * PlayerController) override;

	/* �ؽ�Ʈ Ŀ�Ե� �� ȣ�� ��
	 *
	 * @param		InText			Ŀ���� �ؽ�Ʈ
	 * @param		InCommitInfo	Ŀ�Կ� ���� ���� (ex. ����Ű�� �Է¹���, ��Ŀ���� �ٲ� ��)
	 */
	UFUNCTION(BlueprintCallable)
	void OnChatTextCommitted(const FText& InText, ETextCommit::Type InCommitInfo);

	/* ä�� ����� �߰� �Ѵ� */
	void AddChatLine(const FText& ChatString);

	
};
