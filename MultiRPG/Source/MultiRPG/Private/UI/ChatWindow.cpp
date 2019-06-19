// Fill out your copyright notice in the Description page of Project Settings.

#include "ChatWindow.h"
#include "Runtime/UMG/Public/Components/EditableTextBox.h"
#include "Runtime/Core/Public/UObject/ScriptDelegates.h"
#include "Components/ScrollBox.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "..\..\Public\UI\ChatWindow.h"

UChatWindow::UChatWindow(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void UChatWindow::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UChatWindow::Setup(APlayerController * PlayerController)
{
	Super::Setup(PlayerController);

	if (ChatEntry->OnTextCommitted.IsBound())
	{
		/* 기존 바인딩 걸려있는 HandleOnTextCommitted() 함수를 제거한다 */
		// 이걸 근본적으로 깔끔하게 바꿀려면 RebuildWidget()을 오버라이딩 하거나,
		// Blueprint Widget을 쓰지 말고 c++ 코드로 위젯을 생성하면
		// 위젯을 생성자 할 때 설정 할 수 있다
		ChatEntry->OnTextCommitted.Clear();
	}

	// OnChatTextCommitted() 함수로 바인딩 설정
	TScriptDelegate<FWeakObjectPtr> Delegate;
	Delegate.BindUFunction(this, FName("OnChatTextCommitted"));
	ChatEntry->OnTextCommitted.Add(Delegate);

	this->AddToViewport();
}

void UChatWindow::OnChatTextCommitted(const FText & InText, ETextCommit::Type InCommitInfo)
{
	if (InCommitInfo == ETextCommit::OnEnter)
	{
		AMultiRPGPlayerController* PlayerController = Cast<AMultiRPGPlayerController>(GetPlayerController());

		if (PlayerController != nullptr && !InText.IsEmpty())
		{
			// 서버로 메세지를 보낸다
			PlayerController->SendText(InText.ToString());

			if (ChatEntry)
			{
				// 채팅 기록을 추가한다
				AddChatLine(InText);
				ChatEntry->SetText(FText());
			}
		}

		// 포커스를 UI에서 게임전용으로 변경 한다
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
	}
}

void UChatWindow::AddChatLine(const FText& ChatString)
{
	if (ChatHistory)
	{
		// TextBlock을 생성한다
		UTextBlock* ChatLine = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
		if (ChatLine)
		{
			// Text 를 설정하고 자동 줄 바꿈 옵션을 킨다
			ChatLine->SetText(ChatString);
			ChatLine->SetAutoWrapText(true);

			ChatHistory->AddChild(ChatLine);
			// 스크롤을 마지막으로 내린다
			ChatHistory->ScrollToEnd();
		}
	}
}

void UChatWindow::ToggleTextBoxKeyboardFocus()
{
	if (!ChatEntry->HasKeyboardFocus())
	{
		ChatEntry->SetKeyboardFocus();
		UE_LOG(LogClass, Log, TEXT("@@@@@ SetKeyboardFocus"));
	}
}
