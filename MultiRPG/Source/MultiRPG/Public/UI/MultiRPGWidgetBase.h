// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Public/Player/MultiRPGPlayerController.h"
#include "MultiRPGWidgetBase.generated.h"

UCLASS()
class MULTIRPG_API UMultiRPGWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UMultiRPGWidgetBase(const FObjectInitializer& ObjectInitializer);

public:
	/* ���� �ʱ�ȭ �Լ� */
	virtual void Setup(APlayerController * PlayerController);

	/* ���� �����ϰ� �ִ� ��Ʈ�ѷ��� ��ȯ �Ѵ� */
	TWeakObjectPtr<AMultiRPGPlayerController> GetPlayerController() const;

protected:
	/* ������ �����ϰ� �ִ� ��Ʈ�ѷ� */
	TWeakObjectPtr<AMultiRPGPlayerController> OwnerController;

};
