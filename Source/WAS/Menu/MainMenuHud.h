// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHud.generated.h"

class UMainMenuWidget;

/**
 * 
 */
UCLASS()
class WAS_API AMainMenuHud : public AHUD
{
	GENERATED_BODY()

public:
	void CreateNewWidget(TSubclassOf<UMainMenuWidget>& mainMenuWidget);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UMainMenuWidget> StartMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	UMainMenuWidget* HUDOverlay;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
};
