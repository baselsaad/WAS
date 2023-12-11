// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UInventoryButtonWidget::OnClick()
{
	InventoryWidget->OpenArticle(Title, Content, BackgroundImage);
}

void UInventoryButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	InventoryButtonText->SetText(FText::FromString(Title));
	InventoryButton->OnClicked.AddDynamic(this, &UInventoryButtonWidget::OnClick);
}

void UInventoryButtonWidget::NativeDestruct()
{
	Super::NativeDestruct();
}
