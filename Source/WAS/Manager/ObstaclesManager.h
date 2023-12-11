// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstaclesManager.generated.h"

/*************************************************************
*		MAKE ONLY ONE INSTANCE (Blueprint) OF THIS IN  		 *
*		THE SCENE TO WORK CORRECT WITH SAVE AND LOAD SYSTEM	 *
***************************************************************/

USTRUCT()
struct FObstaclesWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<class AActor*> Obstacles;
};


UCLASS()
class WAS_API AObstaclesManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObstaclesManager();

public:	
	// Show and Hide Obstacles based of the run of the player
	UPROPERTY(EditAnywhere, meta = (DisplayName = "ToShow", AllowPrivateAccess = true))
	TMap<int, FObstaclesWrapper> ToShow;
};


