// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class WAS_API ADoor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADoor();

	virtual void Tick(float DeltaSeconds) override;
	void RaiseWall(float deltaTime);
	void LowerWall(float deltaTime);

	UFUNCTION()
	void OverlapBeginn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor
		, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FVector DoorMoveDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float DoorMoveDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float DoorRotateAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool OpenByRotate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool SlideOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	class ABasePuzzle* AttatchedPuzzle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool CanOpen; // Always true unless there is specific puzzle should set it to true and the default value should be in editor set to false

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool EndDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FString HintTextToShow;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	FTimerHandle m_AnimationTimer;
	bool m_ShouldOpen;
	FVector m_DoorLocation;

};
