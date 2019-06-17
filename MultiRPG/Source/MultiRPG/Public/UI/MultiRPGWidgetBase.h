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
	/* 위젯 초기화 함수 */
	virtual void Setup(APlayerController * PlayerController);

	/* 현재 소유하고 있는 컨트롤러를 반환 한다 */
	TWeakObjectPtr<AMultiRPGPlayerController> GetPlayerController() const;

protected:
	/* 위젯을 소유하고 있는 컨트롤러 */
	TWeakObjectPtr<AMultiRPGPlayerController> OwnerController;

};
