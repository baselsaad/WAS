// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "Pill.generated.h"

/**
 *
 */
UCLASS()
class WAS_API APill : public ABaseItem
{
	GENERATED_BODY()

public:
	APill();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Pill;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBoxForInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ispilltaken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int UniqueID;

	virtual void PickUP(AScientist* mainPlayer) override;
private:
	void Onpickup(AScientist* mainPlayer);
	void PlayDelayedVoiceLine(AScientist* mainPlayer);
};
