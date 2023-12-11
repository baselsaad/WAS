// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/InventoryWidget.h"
#include <Components/VerticalBox.h>
#include <Components/TextBlock.h>
#include "Debug.h"
#include "MyAssertTool.h"
#include "Scientist.h"
#include <Kismet/GameplayStatics.h>
#include <Components/MultiLineEditableTextBox.h>
#include <Components/Image.h>

#include "InventoryButtonWidget.h"

void UInventoryWidget::UpdateItems()
{
	if (ArticleButtonWrapper == nullptr)
		return;

	Debug::LogWarn("UpdateItems");
	AScientist* player = Cast<AScientist>(UGameplayStatics::GetPlayerPawn(this, 0));
	ASSERT_BREAK(player, "player is null!!");

	TSet<FString> outTitles;
	m_Articles.GetKeys(outTitles);

	KeysCollected->SetText(FText::Format(FText::FromString("{0} Schluesselteile im Inventar"),
	                                     player->GetInventory().CollectedKeys.Num()));

	Debug::LogWarn("Keys: %d", player->GetInventory().CollectedKeys.Num());

	ArticlesCollected->SetText(FText::Format(FText::FromString("Artikel ({0}):"),
	                                     player->GetInventory().CollectedArticles.Num()));

	for (auto article : player->GetInventory().CollectedArticles)
	{
		if (!outTitles.Find(article->Title))
		{
			AddNewArticle(article->Title, article->Content, article->BackgroundImage);
		}
	}
}

void UInventoryWidget::AddNewArticle(FString title, FString content, UTexture2D* background)
{
	m_Articles.Add(title, content);

	UInventoryButtonWidget* Widget = Cast<UInventoryButtonWidget>(CreateWidget(this, ButtonWidgetClass));
	if (Widget)
	{
		Widget->InventoryWidget = this;
		Widget->Title = title;
		Widget->Content = content;
		Widget->BackgroundImage = background;
		ArticleButtonWrapper->AddChild(Widget);
	}
}

void UInventoryWidget::OpenArticle(FString articleTitle, FString articleContent, UTexture2D* backgroundtex)
{
	Title->SetText(FText::FromString(articleTitle));
	Content->SetText(FText::FromString(articleContent));
	Background->SetBrushFromTexture(backgroundtex);
	Background->Brush.DrawAs = ESlateBrushDrawType::Image;
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryWidget::NativeDestruct()
{
	Super::NativeDestruct();
}
