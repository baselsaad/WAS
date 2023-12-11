// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "OxygenTank.generated.h"

class AMask;
class AScientist;

UCLASS()
class WAS_API AOxygenTank : public AActor
{
	GENERATED_BODY()

public:
	AOxygenTank();

	UFUNCTION()
		void OverlapBeginn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor
			, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* OxygenTank;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* CollisionBoxForInteraction;

	UFUNCTION()
		void InteractWithMask(AMask* Mask);


	AScientist* Player;

	void BeginPlay();
	void Tick(float DeltaTime);
	 
};