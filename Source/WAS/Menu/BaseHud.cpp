// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/BaseHud.h"
#include "Menu/InventoryWidget.h"

#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "GameHudWidget.h"
#include "CanvasItem.h"

void ABaseHud::DrawCrossHair()
{
	if (CrosshairTex)
	{
		// find center of the Canvas
		const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

		// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
		const FVector2D CrosshairDrawPosition((Center.X), (Center.Y));

		// draw the cross hair
		FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
		TileItem.BlendMode = SE_BLEND_Translucent;
		TileItem.Size = FVector2D(10.0f, 10.0f);
		Canvas->DrawItem(TileItem);
	}
}

void ABaseHud::CreateNewWidget(TSubclassOf<UGameHudWidget>& gameWidget)
{
	HUDOverlay = CreateWidget<UGameHudWidget>(GetWorld(), gameWidget);
	HUDOverlay->AddToViewport();
	HUDOverlay->SetOwningPlayer(GetOwningPlayerController());
}

void ABaseHud::ShowInventoryWidget()
{
	if (m_InventoryOverlay == nullptr)
	{
		m_InventoryOverlay = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidget);
		m_InventoryOverlay->AddToViewport();
		m_InventoryOverlay->SetOwningPlayer(GetOwningPlayerController());
	}

	m_InventoryOverlay->UpdateItems();
	m_InventoryOverlay->SetVisibility(ESlateVisibility::Visible);
	// Update Data in Widget if needed
}

void ABaseHud::HideInventoryWidget()
{
	m_InventoryOverlay->SetVisibility(ESlateVisibility::Hidden);
}

void ABaseHud::DrawHUD()
{
	Super::DrawHUD();

	DrawCrossHair();
}

UInventoryWidget* ABaseHud::GetInventoryWidget()
{
	if (m_InventoryOverlay == nullptr)
	{
		m_InventoryOverlay = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidget);
		m_InventoryOverlay->AddToViewport();
		m_InventoryOverlay->SetVisibility(ESlateVisibility::Hidden);
		m_InventoryOverlay->SetOwningPlayer(GetOwningPlayerController());
	}

	return m_InventoryOverlay;
}

void ABaseHud::BeginPlay()
{
	Super::BeginPlay();

	CreateNewWidget(StartMenu);
}

void ABaseHud::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
