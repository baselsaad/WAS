// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHud.generated.h"

class UGameHudWidget;
class UInventoryWidget;
/**
 *
 */
UCLASS()
class WAS_API ABaseHud : public AHUD
{
	GENERATED_BODY()

public:
	void DrawCrossHair();
	void CreateNewWidget(TSubclassOf<UGameHudWidget>& gameWidget);
	void ShowInventoryWidget();
	void HideInventoryWidget();
	virtual void DrawHUD() override;

	UInventoryWidget* GetInventoryWidget();
	

public:
	UPROPERTY(EditAnywhere, Category = "Level")
	class UTexture2D* CrosshairTex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UGameHudWidget> StartMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryWidget")
	TSubclassOf<UInventoryWidget> InventoryWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	UGameHudWidget* HUDOverlay;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UInventoryWidget* m_InventoryOverlay;
};
