// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/OptionsWidget.h"

#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include <Kismet/GameplayStatics.h>
#include "Player/Scientist.h"
#include "Player/ScientistController.h"
#include "LevelManager.h"
#include "Debug.h"


void UOptionsWidget::Quit()
{
	UMainMenuWidget::Get()->CloseOptions();
}

void UOptionsWidget::OnQualityBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GraphicsOption op = GraphicsOption::High;

	if (SelectedItem == "Epic")
		op = GraphicsOption::Epic;
	else if (SelectedItem == "Hoch")
		op = GraphicsOption::High;
	else if (SelectedItem == "Mittel")
		op = GraphicsOption::Med;
	else if (SelectedItem == "Niedrig")
		op = GraphicsOption::Low;

	AScientistController::SetQuality(op);
	auto levelManager = Cast<ULevelManager>(UGameplayStatics::GetGameInstance(this));
	if (levelManager)
	{
		levelManager->SaveGraphicsSettings(op);
	}
}

void UOptionsWidget::OnShadowBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	GraphicsOption op = GraphicsOption::High;

	if (SelectedItem == "Epic")
		op = GraphicsOption::Epic;
	else if (SelectedItem == "Hoch")
		op = GraphicsOption::High;
	else if (SelectedItem == "Mittel")
		op = GraphicsOption::Med;
	else if (SelectedItem == "Niedrig")
		op = GraphicsOption::Low;

	AScientistController::SetShadowQuality(op);
	auto levelManager = Cast<ULevelManager>(UGameplayStatics::GetGameInstance(this));
	if (levelManager)
	{
		levelManager->SaveShadowSettings(op);
	}
}

void UOptionsWidget::OnResBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	FIntPoint res(1920, 1080);

	if (SelectedItem == "3840 x 2160")
		res = FIntPoint(3840, 2160);
	else if (SelectedItem == "2560 x 1440")
		res = FIntPoint(2560, 1440);
	else if (SelectedItem == "1920 x 1080")
		res = FIntPoint(1920, 1080);
	else if (SelectedItem == "1280 x 720")
		res = FIntPoint(1280, 720);

	AScientistController::SetScreenRes(res);
	auto levelManager = Cast<ULevelManager>(UGameplayStatics::GetGameInstance(this));
	if (levelManager)
	{
		levelManager->SaveRes(res);
	}
}

void UOptionsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TextureQuality->AddOption("Epic");
	TextureQuality->AddOption("Hoch");
	TextureQuality->AddOption("Mittel");
	TextureQuality->AddOption("Niedrig");
	TextureQuality->SetSelectedIndex(0);
	TextureQuality->OnSelectionChanged.AddDynamic(this, &UOptionsWidget::OnQualityBoxSelectionChanged);


	ShadowMapQuality->AddOption("Epic");
	ShadowMapQuality->AddOption("Hoch");
	ShadowMapQuality->AddOption("Mittel");
	ShadowMapQuality->AddOption("Niedrig");
	ShadowMapQuality->SetSelectedIndex(0);
	ShadowMapQuality->OnSelectionChanged.AddDynamic(this, &UOptionsWidget::OnShadowBoxSelectionChanged);

	Resolution->AddOption("3840 x 2160");
	Resolution->AddOption("2560 x 1440");
	Resolution->AddOption("1920 x 1080");
	Resolution->AddOption("1280 x 720");
	Resolution->SetSelectedIndex(0);
	Resolution->OnSelectionChanged.AddDynamic(this, &UOptionsWidget::OnResBoxSelectionChanged);

	Volume->SetValue(1);
	MouseSensitivity->SetValue(0.5);

	QuitButton->OnClicked.AddDynamic(this, &UOptionsWidget::Quit);


	auto levelManager = Cast<ULevelManager>(UGameplayStatics::GetGameInstance(this));
	if (levelManager)
	{
		switch (levelManager->LoadGraphicsSettings())
		{
			case GraphicsOption::Low:	TextureQuality->SetSelectedIndex(3);	break;
			case GraphicsOption::Med:	TextureQuality->SetSelectedIndex(2);	break;
			case GraphicsOption::High:	TextureQuality->SetSelectedIndex(1);	break;
			case GraphicsOption::Epic:	TextureQuality->SetSelectedIndex(0);	break;
			default:					TextureQuality->SetSelectedIndex(0);	break;
		}

		switch (levelManager->LoadShadowSettings())
		{
			case GraphicsOption::Low:	ShadowMapQuality->SetSelectedIndex(3);	break;
			case GraphicsOption::Med:	ShadowMapQuality->SetSelectedIndex(2);	break;
			case GraphicsOption::High:	ShadowMapQuality->SetSelectedIndex(1);	break;
			case GraphicsOption::Epic:	ShadowMapQuality->SetSelectedIndex(0);	break;
			default:					ShadowMapQuality->SetSelectedIndex(0);	break;
		}

		switch (levelManager->GetSavedRes().Y)
		{
			case 2160:	Resolution->SetSelectedIndex(0);	break;
			case 1440:	Resolution->SetSelectedIndex(1);	break;
			case 1080:	Resolution->SetSelectedIndex(2);	break;
			case 720:	Resolution->SetSelectedIndex(3);	break;
			default:	Resolution->SetSelectedIndex(2);	break;
				break;
		}
	}
}

void UOptionsWidget::NativeDestruct()
{
	Super::NativeDestruct();
}