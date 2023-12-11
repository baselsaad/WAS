// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAS_API UOptionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* TextureQuality;

	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* ShadowMapQuality;

	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* Resolution;

	UPROPERTY(meta=(BindWidget))
	class UCheckBox* PostProcessing;

	UPROPERTY(meta=(BindWidget))
	class USlider* Volume;

	UPROPERTY(meta=(BindWidget))
	class USlider* MouseSensitivity;

	UPROPERTY(meta=(BindWidget))
	class UButton* QuitButton;

	UFUNCTION()
	void Quit();


	UFUNCTION()
	void OnQualityBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnShadowBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnResBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);


protected:
	virtual void NativeConstruct() override; //begin play
	virtual void NativeDestruct() override; //End play
};
