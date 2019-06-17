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

	/* 서버로 채팅 메세지를 보낸다
	 *
	 * @param	Text	채팅 메세지
	 */
	void SendText(const FString& Msg);
	UFUNCTION(Reliable, Server, WithValidation)
	virtual void ServerRPCSendText(const FString& Msg);
	void ServerRPCSendText_Implementation(const FString& Msg);
	bool ServerRPCSendText_Validate(const FString& Msg);

	/* 클라이언트 메세지를 받는 부분 (Client 전용) */
	virtual void ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime = 0) override;

	/* 위젯 매니저를 가져옴 */
	AWidgetManager* GetWidgetManager() const;

	/* 채팅 윈도우를 보여줌 */
	void OnChatWindow();
private:

};
