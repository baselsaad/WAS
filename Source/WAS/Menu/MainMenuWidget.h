// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UTestWidget;
class UMainMenuSecondaryButtonWidget;
class UOptionsWidget;

/**
 * 
 */
UCLASS()
class WAS_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	static UMainMenuWidget* Get();
	void SetPrimaryButton(FText Caption, int Action);
	UMainMenuSecondaryButtonWidget* AddSecondaryButton(FText caption, int Action);

	UPROPERTY(meta=(BindWidget))
	class UButton* PrimaryButton;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* PrimaryButtonText;

	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* SecondaryButtonWrapper;

	UPROPERTY(meta=(BindWidget))
	class UBorder* MainMenuWrapper;

	UPROPERTY(meta=(BindWidget))
	class UBorder* LoadingBar;

	UPROPERTY(EditAnywhere, Category="Class Types")
	TSubclassOf<UMainMenuSecondaryButtonWidget> ButtonWidgetClass;

	UPROPERTY(EditAnywhere, Category="Class Types")
	TSubclassOf<UOptionsWidget> OptionsWidgetClass;

	UPROPERTY(VisibleInstanceOnly, Category="Runtime")
	class UMainMenuSecondaryButtonWidget* MainMenuSecondaryButtonWidget;

	UFUNCTION()
	void StartNewGame();

	UFUNCTION()
	void ResumeLatestGame();

	UFUNCTION()
	void OpenLoadGame();

	UFUNCTION()
	void OpenOptions();

	UFUNCTION()
	void CloseOptions();

	UFUNCTION()
	void QuitGame();
	
protected:
	virtual void NativeConstruct() override; //begin play
	virtual void NativeDestruct() override; //End play
};
