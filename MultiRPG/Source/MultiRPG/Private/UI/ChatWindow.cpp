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
		/* ���� ���ε� �ɷ��ִ� HandleOnTextCommitted() �Լ��� �����Ѵ� */
		// �̰� �ٺ������� ����ϰ� �ٲܷ��� RebuildWidget()�� �������̵� �ϰų�,
		// Blueprint Widget�� ���� ���� c++ �ڵ�� ������ �����ϸ�
		// ������ ������ �� �� ���� �� �� �ִ�
		ChatEntry->OnTextCommitted.Clear();
	}

	// OnChatTextCommitted() �Լ��� ���ε� ����
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
			// ������ �޼����� ������
			PlayerController->SendText(InText.ToString());

			if (ChatEntry)
			{
				// ä�� ����� �߰��Ѵ�
				AddChatLine(InText);
				ChatEntry->SetText(FText());
			}
		}

		// ��Ŀ���� UI���� ������������ ���� �Ѵ�
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
	}
}

void UChatWindow::AddChatLine(const FText& ChatString)
{
	if (ChatHistory)
	{
		// TextBlock�� �����Ѵ�
		UTextBlock* ChatLine = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
		if (ChatLine)
		{
			// Text �� �����ϰ� �ڵ� �� �ٲ� �ɼ��� Ų��
			ChatLine->SetText(ChatString);
			ChatLine->SetAutoWrapText(true);

			ChatHistory->AddChild(ChatLine);
			// ��ũ���� ���������� ������
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
