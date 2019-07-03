// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Public/UI/WidgetName.h"
#include "Public/UI/WidgetDepth.h"
#include "UMG/Public/Components/SlateWrapperTypes.h"
#include "Public/UI/ChatWindow.h"
#include "Public/UI/HUDCrosshair.h"
#include "WidgetManager.generated.h"

class UMultiRPGWidgetBase;

UCLASS()
class MULTIRPG_API AWidgetManager : public AHUD
{
	GENERATED_BODY()

public:
	AWidgetManager(const FObjectInitializer& ObjectInitializer);

public:
	virtual void PostInitializeComponents() override;

	/*
	 * Widget이 존재하는지(생성 됐는지) 검사함
	 *
	 * @Param	WidgetName	Widget 이름 (WidgetName)
	 */
	bool IsValidWidget(const FString& WidgetName);

	/*
	 * 문자열을 ChatWindow에 추가함 
	 *
	 * @param	ChatString	추가할 문자열
	 */
	void AddChatLine(const FText& ChatString);

	/* ChatWindow TextBox에 키보드 포커스 설정 */
	void ToggleChatWindow();

	template <typename T>
	void OpenWidget(const FString& WidgetName, const EWidgetDepth Type);

protected:
	//class UChatWindow* ChatWindow;
	class UHUDCrosshair* HudCrosshair;
	TMap<FString, FString> WidgetDefinitions;
	TMap<FString, UMultiRPGWidgetBase*> HUDLoadedMap;
};

/** 위젯을 열고 보여줌
 *
 * @Param	WidgetName	위젯 이름 (Key)
 */
template<typename T>
inline void AWidgetManager::OpenWidget(const FString & WidgetName, const EWidgetDepth Type)
{
	AMultiRPGPlayerController* PC = Cast<AMultiRPGPlayerController>(PlayerOwner);
	if (PC)
	{
		if (!HUDLoadedMap.Find(WidgetName))
		{
			FStringClassReference WidgetClassRef(WidgetDefinitions[WidgetName]);
			UClass * WidgetClass = WidgetClassRef.TryLoadClass <T>();

			if (WidgetClass)
			{
				T* Target = CreateWidget<T>(GetWorld(), WidgetClass);

				if (Target)
				{
					Target->Setup(PC);
					Target->SetVisibility(ESlateVisibility::Visible);

					switch (Type)
					{
						case EWidgetDepth::HUD:
						case EWidgetDepth::OVERLAY:
						{
							HUDLoadedMap.Emplace(WidgetName, Target);
							break;
						}
						case EWidgetDepth::WINDOW:
							break;
					}
					
				}
			}
		}
	}
}
