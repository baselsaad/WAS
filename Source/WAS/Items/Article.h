// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"

#include "Article.generated.h"

class UBoxComponent;

/**
 *
 */
UCLASS()
class WAS_API AArticle : public ABaseItem
{
	GENERATED_BODY()


public:
	AArticle();


	virtual void PickUP(AScientist* mainPlayer) override;
	void OpenPaper();


	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* BackgroundImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle")
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float m_DistanceFromCamera;

	bool IsPaperOpend;


	UFUNCTION()
	void OverlapBeginn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "sound")
		class USoundBase* Newspaper;

};
