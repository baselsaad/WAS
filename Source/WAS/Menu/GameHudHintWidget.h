// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHudHintWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAS_API UGameHudHintWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	void SetKeyImage(FString Path);

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Key;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Hint;

	UPROPERTY(meta=(BindWidget))
	class UBorder* KeyBorderImg;

	void SetKeyboardHint(FText HintKey, FText HintText);
	void SetKeyboardHintText(FText HintKey, FText HintText);
};
