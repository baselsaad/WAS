// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/MainMenuWidget.h"
#include "Debug.h"
#include "InventoryButtonWidget.h"
#include "MainMenuSecondaryButtonWidget.h"
#include "OptionsWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "LevelManager.h"

static TSoftObjectPtr<UMainMenuWidget> s_MainMenuInstance = nullptr;
TSoftObjectPtr<UOptionsWidget> options_Instance = nullptr;

UMainMenuWidget* UMainMenuWidget::Get()
{
	if (s_MainMenuInstance == nullptr)
	{
		Debug::LogWarn("s_Instance is null!");
	}

	return s_MainMenuInstance.Get();
}

void UMainMenuWidget::SetPrimaryButton(FText Caption, int Action)
{
	PrimaryButtonText->SetText(Caption);

	switch (Action)
	{
	case 0:
		PrimaryButton->OnClicked.AddDynamic(this, &UMainMenuWidget::StartNewGame);
		break;
	case 1:
		PrimaryButton->OnClicked.AddDynamic(this, &UMainMenuWidget::ResumeLatestGame);
		break;
	default:
		Debug::LogWarn("Action not found!");
		break;
	}
}

UMainMenuSecondaryButtonWidget* UMainMenuWidget::AddSecondaryButton(FText Caption, int Action)
{
	UMainMenuSecondaryButtonWidget* Widget = Cast<
		UMainMenuSecondaryButtonWidget>(CreateWidget(this, ButtonWidgetClass));
	Widget->SecondaryButtonText->SetText(Caption);

	switch (Action)
	{
	case 0:
		Widget->SecondaryButton->OnClicked.AddDynamic(Widget, &UMainMenuSecondaryButtonWidget::StartNewGame);
		break;
	case 1:
		Widget->SecondaryButton->OnClicked.AddDynamic(Widget, &UMainMenuSecondaryButtonWidget::ResumeLatestGame);
		break;
	case 2:
		Widget->SecondaryButton->OnClicked.AddDynamic(Widget, &UMainMenuSecondaryButtonWidget::OpenLoadGame);
		break;
	case 3:
		Widget->SecondaryButton->OnClicked.AddDynamic(Widget, &UMainMenuSecondaryButtonWidget::OpenOptions);
		break;
	case 4:
		Widget->SecondaryButton->OnClicked.AddDynamic(Widget, &UMainMenuSecondaryButtonWidget::QuitGame);
		break;
	default:
		Debug::LogWarn("Action not found!");
		break;
	}


	SecondaryButtonWrapper->AddChild(Widget);
	return Widget;
}

void UMainMenuWidget::StartNewGame()
{
	MainMenuWrapper->SetVisibility(ESlateVisibility::Hidden);
	LoadingBar->SetVisibility(ESlateVisibility::Visible);

	ULevelManager* levelmanager = Cast<ULevelManager>(GetGameInstance());
	if (levelmanager)
	{
		levelmanager->ResetGame();
	}
	
	static FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		UGameplayStatics::OpenLevel(GEngine->GameViewport->GetWorld(), FName("Demonstration"));
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}, 0.2, false);
}

void UMainMenuWidget::ResumeLatestGame()
{
	Debug::LogWarn("Resume Game");
}

void UMainMenuWidget::OpenLoadGame()
{
	Debug::LogWarn("Load Game");

	MainMenuWrapper->SetVisibility(ESlateVisibility::Hidden);
	LoadingBar->SetVisibility(ESlateVisibility::Visible);

	static FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			UGameplayStatics::OpenLevel(GEngine->GameViewport->GetWorld(), FName("Demonstration"));
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		}, 0.2, false);

}

void UMainMenuWidget::OpenOptions()
{
	options_Instance = Cast<UOptionsWidget>(CreateWidget(this, OptionsWidgetClass));
	MainMenuWrapper->SetVisibility(ESlateVisibility::Hidden);
	options_Instance->AddToViewport();
}

void UMainMenuWidget::CloseOptions()
{
	options_Instance->RemoveFromViewport();
	MainMenuWrapper->SetVisibility(ESlateVisibility::Visible);
	options_Instance.Reset();
	options_Instance = nullptr;
}

void UMainMenuWidget::QuitGame()
{
	FGenericPlatformMisc::RequestExit(false);
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	s_MainMenuInstance.Reset();
	s_MainMenuInstance = this;
	LoadingBar->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();

	/*if (s_MainMenuInstance)
		s_MainMenuInstance.Reset();*/
}
