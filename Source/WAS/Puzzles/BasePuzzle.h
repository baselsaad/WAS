// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UCameraComponent;
class UBoxComponent;
class AScientist;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePuzzle.generated.h"

UCLASS()
class WAS_API ABasePuzzle : public AActor
{
	GENERATED_BODY()
	
public:	
	ABasePuzzle();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Interact(AScientist* mainPlayer);
	virtual void Update();

	UFUNCTION(BlueprintCallable)
	virtual bool IsSolved();

	virtual void OnEnterPuzzle();
	virtual void OnLeavePuzzle();


	// if the puzzle does not need a camera
	void DestroyCamera();
	void MouseCursor(bool active);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* m_PuzzleCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* m_Trigger;

	TSoftObjectPtr<AScientist> m_MainPlayerPtr;
};
