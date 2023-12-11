// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu/GameHudWidget.h"
#include "GameHudHintWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/ProgressBar.h"
#include "Debug.h"



// s_Instance ist private und kann nur in GameHudWidget.cpp benutzt werden
static TSoftObjectPtr<UGameHudWidget> s_GameHudWidgetInstance = nullptr;
FTimerHandle OxygenTimerHandle;

UGameHudWidget* UGameHudWidget::Get()
{
	if (s_GameHudWidgetInstance == nullptr)
	{
		Debug::LogWarn("s_Instance is null!!");
	}

	return s_GameHudWidgetInstance.Get();
}

UGameHudHintWidget* UGameHudWidget::AddDialogText(FText Key, FText Text)
{
	UGameHudHintWidget* Widget = Cast<UGameHudHintWidget>(CreateWidget(this, GameHintClass));
	Widget->SetKeyboardHint(Key, Text);
	DialogWrapper->AddChild(Widget);
	UVerticalBoxSlot* WidgetSlot = Cast<UVerticalBoxSlot>(Widget->Slot);
	WidgetSlot->SetHorizontalAlignment(HAlign_Center);
	WidgetSlot->SetVerticalAlignment(VAlign_Bottom);
	WidgetSlot->SetSize(ESlateSizeRule::Fill);
	return Widget;
}

UGameHudHintWidget* UGameHudWidget::AddDialogText(FText Key, FText Text, int Duration)
{
	UGameHudHintWidget* Widget = AddDialogText(Key, Text);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Widget, TimerHandle]()
	{
		if (Widget && GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
		{
			Widget->RemoveFromParent();
		}
	}, Duration, false);

	return Widget;
}

UGameHudHintWidget* UGameHudWidget::AddHintText(FText Text)
{
	UGameHudHintWidget* Widget = Cast<UGameHudHintWidget>(CreateWidget(this, GameHintClass));
	Widget->SetKeyboardHintText(FText::FromString(""), Text);
	HintWrapper->AddChild(Widget);
	UVerticalBoxSlot* WidgetSlot = Cast<UVerticalBoxSlot>(Widget->Slot);
	//WidgetSlot->SetHorizontalAlignment(HAlign_Center);
	//WidgetSlot->SetVerticalAlignment(VAlign_Bottom);
	WidgetSlot->SetSize(ESlateSizeRule::Fill);
	return Widget;
}

void UGameHudWidget::ClearHintText()
{
	HintWrapper->ClearChildren();
}

void UGameHudWidget::ClearDialogText()
{
	DialogWrapper->ClearChildren();
}

void UGameHudWidget::SetOxygenBar(float Value)
{
	OxygenBar->SetPercent(Value * 0.01);
}

void UGameHudWidget::NativeConstruct()
{
	Super::NativeConstruct();

	s_GameHudWidgetInstance.Reset();
	s_GameHudWidgetInstance = this;
}

void UGameHudWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (s_GameHudWidgetInstance)
		s_GameHudWidgetInstance.Reset();
}
