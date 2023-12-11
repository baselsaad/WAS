// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "flashlight.generated.h"

/**
 *
 */
UCLASS()
class WAS_API Aflashlight : public ABaseItem
{
	GENERATED_BODY()

public:
	Aflashlight();

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* flashlight;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* CollisionBoxForInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool ispilltaken;

	virtual void  PickUP(AScientist* mainPlayer) override;
private:
	void Onpickup(AScientist* mainPlayer);


};
