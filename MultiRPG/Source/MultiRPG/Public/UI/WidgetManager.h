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
	 * Widget�� �����ϴ���(���� �ƴ���) �˻���
	 *
	 * @Param	WidgetName	Widget �̸� (WidgetName)
	 */
	bool IsValidWidget(const FString& WidgetName);

	/*
	 * ���ڿ��� ChatWindow�� �߰��� 
	 *
	 * @param	ChatString	�߰��� ���ڿ�
	 */
	void AddChatLine(const FText& ChatString);

	/* ChatWindow TextBox�� Ű���� ��Ŀ�� ���� */
	void ToggleChatWindow();

	template <typename T>
	void OpenWidget(const FString& WidgetName, const EWidgetDepth Type);

protected:
	//class UChatWindow* ChatWindow;
	class UHUDCrosshair* HudCrosshair;
	TMap<FString, FString> WidgetDefinitions;
	TMap<FString, UMultiRPGWidgetBase*> HUDLoadedMap;
};

/** ������ ���� ������
 *
 * @Param	WidgetName	���� �̸� (Key)
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
