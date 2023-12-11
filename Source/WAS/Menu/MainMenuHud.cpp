// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/MainMenuHud.h"

#include "MainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "LevelManager.h"
#include "Engine/Texture2D.h"

void AMainMenuHud::CreateNewWidget(TSubclassOf<UMainMenuWidget>& mainMenuWidget)
{
	HUDOverlay = CreateWidget<UMainMenuWidget>(GetWorld(), mainMenuWidget);
	HUDOverlay->AddToViewport();
	HUDOverlay->SetOwningPlayer(GetOwningPlayerController());
}

void AMainMenuHud::BeginPlay()
{
	Super::BeginPlay();

	CreateNewWidget(StartMenu);

	/*if (GetWorld()->GetGameInstanceChecked<ULevelManager>()->HasSavedGame())
	{
		HUDOverlay->SetPrimaryButton(FText::FromString("Fortsetzen"), 1);

		HUDOverlay->AddSecondaryButton(FText::FromString("Neues Spiel"), 0);
		HUDOverlay->AddSecondaryButton(FText::FromString("Spiel laden"), 2);
		HUDOverlay->AddSecondaryButton(FText::FromString("Optionen"), 3);
		HUDOverlay->AddSecondaryButton(FText::FromString("Beenden"), 4);
	}
	else
	{*/

	//TODO only for demo purposes for milestone 3
	HUDOverlay->SetPrimaryButton(FText::FromString("Neues Spiel"), 0);

	ULevelManager* level = Cast<ULevelManager>(GetGameInstance());
	if (level && level->HasSavedGame())
		HUDOverlay->AddSecondaryButton(FText::FromString("Fortsetzen"), 2);

	HUDOverlay->AddSecondaryButton(FText::FromString("Optionen"), 3);
	HUDOverlay->AddSecondaryButton(FText::FromString("Beenden"), 4);
	//}

	GetOwningPlayerController()->bShowMouseCursor = true;
}

void AMainMenuHud::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
