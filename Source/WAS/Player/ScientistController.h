// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ScientistController.generated.h"


class UCameraComponent;

UENUM()
enum class GraphicsOption
{
	Low = 0, Med = 1, High = 2, Epic = 3
};

/**
 * 
 */
UCLASS()
class WAS_API AScientistController : public APlayerController
{
	GENERATED_BODY()

public:
	void EnableDepthOfFieldOnObject(UCameraComponent* camera, float distance);
	void DisableDepthOfField(UCameraComponent* camera);

	static void SetQuality(GraphicsOption op);
	static void SetShadowQuality(GraphicsOption op);
	static void SetScreenRes(const FIntPoint& res);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	GraphicsOption Quality;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	int32 Scale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	float FPS;

	UFUNCTION(BlueprintCallable)
	void NeuesSpiel();

protected:
	virtual void BeginPlay() override;

};
