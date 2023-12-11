
#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UInventoryButtonWidget;
class UVerticalBox;
class UTextBlock;
class UButton;
class AArticle;
/**
 * 
 */
UCLASS()
class WAS_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateItems();
	void AddNewArticle(FString title, FString content, UTexture2D* background);

public:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* ArticleButtonWrapper;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KeysCollected;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ArticlesCollected;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Title;

	//UPROPERTY(meta=(BindWidget))
	//class UTextBlock* Description;

	UPROPERTY(meta=(BindWidget))
	class UMultiLineEditableTextBox* Content;

	UPROPERTY(meta=(BindWidget))
	class UImage* Background;
	
	UPROPERTY(EditAnywhere, Category="Class Types")
	TSubclassOf<UInventoryButtonWidget> ButtonWidgetClass;
	
	UFUNCTION()
	void OpenArticle(FString articleTitle, FString articleContent, class UTexture2D* backgroundtex);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	TMap<FString, FString> m_Articles;
	
	//Button (Title)
	FMargin m_PaddingStyle;
	FLinearColor m_ColorAndOpacity;
	FLinearColor m_Background;
};
