// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MultiRPGWidgetBase.h"
#include "HUDCrosshair.generated.h"

class UImage;

UCLASS()
class MULTIRPG_API UHUDCrosshair : public UMultiRPGWidgetBase
{
	GENERATED_BODY()

public:
	UHUDCrosshair(const FObjectInitializer& ObjectInitializer);

	virtual void Setup(APlayerController * PlayerController) override;
	
	// 블루프린트 UI 변수 (Crosshair Image)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (BindWidget))
	UImage* Crosshair;
};
