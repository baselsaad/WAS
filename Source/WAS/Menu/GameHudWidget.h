// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHudWidget.generated.h"

class UGameHudHintWidget;
class FText;
/**
 * 
 */
UCLASS()
class WAS_API UGameHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UGameHudWidget")
	static UGameHudWidget* Get();
	
	UFUNCTION(BlueprintCallable, Category = "UGameHudWidget")
	UGameHudHintWidget* AddDialogText(FText Key, FText Text);
	
	UGameHudHintWidget* AddDialogText(FText Key, FText Text, int Duration);

	UGameHudHintWidget* AddHintText(FText Text);
	void ClearHintText();
	
	UFUNCTION(BlueprintCallable, Category = "UGameHudWidget")
	void ClearDialogText();

	void SetOxygenBar(float Value);

	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* DialogWrapper;

	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* HintWrapper;

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* OxygenBar;

	UPROPERTY(EditAnywhere, Category="Class Types")
	TSubclassOf<UGameHudHintWidget> GameHintClass;
	
protected:
	virtual void NativeConstruct() override; //begin play
	virtual void NativeDestruct() override; //End play

};
