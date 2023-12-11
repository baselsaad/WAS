// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryWidget.h"
#include "Blueprint/UserWidget.h"
#include "InventoryButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAS_API UInventoryButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FString Title;
	FString Content;
	class UTexture2D* BackgroundImage;
	UInventoryWidget* InventoryWidget;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* InventoryButtonText;

	UPROPERTY(meta=(BindWidget))
	class UButton* InventoryButton;

	UFUNCTION()
	void OnClick();
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
