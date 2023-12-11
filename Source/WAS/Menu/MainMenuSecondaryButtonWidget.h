// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuSecondaryButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAS_API UMainMenuSecondaryButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* SecondaryButtonText;

	UPROPERTY(meta=(BindWidget))
	class UButton* SecondaryButton;
	
	UFUNCTION()
	void StartNewGame();

	UFUNCTION()
	void ResumeLatestGame();

	UFUNCTION()
	void OpenLoadGame();

	UFUNCTION()
	void OpenOptions();

	UFUNCTION()
	void QuitGame();
};
