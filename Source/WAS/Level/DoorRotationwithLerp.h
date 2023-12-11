// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DoorRotationwithLerp.generated.h"

class UBoxComponent;

UCLASS()
class WAS_API ADoorRotationwithLerp : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADoorRotationwithLerp();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Door;

	UPROPERTY(EditAnywhere)
		UBoxComponent* MyBoxComponent;

	// declare overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool Open;
	float RotateValue;
	FRotator DoorRotation;

};