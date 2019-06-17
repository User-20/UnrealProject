// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiRPGPlayerController.generated.h"

class AWidgetManager;

UCLASS(Blueprintable)
class MULTIRPG_API AMultiRPGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMultiRPGPlayerController(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

public:

	/* ������ ä�� �޼����� ������
	 *
	 * @param	Text	ä�� �޼���
	 */
	void SendText(const FString& Msg);
	UFUNCTION(Reliable, Server, WithValidation)
	virtual void ServerRPCSendText(const FString& Msg);
	void ServerRPCSendText_Implementation(const FString& Msg);
	bool ServerRPCSendText_Validate(const FString& Msg);

	/* Ŭ���̾�Ʈ �޼����� �޴� �κ� (Client ����) */
	virtual void ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime = 0) override;

	/* ���� �Ŵ����� ������ */
	AWidgetManager* GetWidgetManager() const;

	/* ä�� �����츦 ������ */
	void OnChatWindow();
private:

};
