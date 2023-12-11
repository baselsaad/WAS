// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/GameHudHintWidget.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"

void UGameHudHintWidget::SetKeyImage(FString Path)
{
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *Path));
	KeyBorderImg->SetBrushFromTexture(Texture);
}

void UGameHudHintWidget::SetKeyboardHint(FText HintKey, FText HintText)
{
	KeyBorderImg->SetVisibility(ESlateVisibility::Visible);
	if (HintKey.EqualTo(FText::FromString("RMB")))
	{
		SetKeyImage(FString("/Game/Blueprints/UI/Mouse_Right_Click"));
		Key->SetText(FText::FromString("  "));
	}
	else if (HintKey.EqualTo(FText::FromString("LMB")))
	{
		SetKeyImage(FString("/Game/Blueprints/UI/Mouse_Left_Click"));
		Key->SetText(FText::FromString("  "));
	}
	else if (HintKey.EqualTo((FText::FromString(""))))
	{
		Key->SetText(FText::FromString(""));
		KeyBorderImg->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetKeyImage(FString("/Game/Blueprints/UI/Key_Border"));
		Key->SetText(HintKey);
	}
	Hint->SetText(HintText);
}

void UGameHudHintWidget::SetKeyboardHintText(FText HintKey, FText HintText)
{
	KeyBorderImg->SetVisibility(ESlateVisibility::Visible);
	if (HintKey.EqualTo(FText::FromString("RMB")))
	{
		SetKeyImage(FString("/Game/Blueprints/UI/Mouse_Right_Click"));
		Key->SetText(FText::FromString("  "));
	}
	else if (HintKey.EqualTo(FText::FromString("LMB")))
	{
		SetKeyImage(FString("/Game/Blueprints/UI/Mouse_Left_Click"));
		Key->SetText(FText::FromString("  "));
	}
	else if (HintKey.EqualTo((FText::FromString(""))))
	{
		Key->SetText(FText::FromString(""));
		KeyBorderImg->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetKeyImage(FString("/Game/Blueprints/UI/Key_Border"));
		Key->SetText(HintKey);
	}
	Hint->SetText(HintText);

	Key->Font.Size = 16;
	Hint->Font.Size = 16;
}
