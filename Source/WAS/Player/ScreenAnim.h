// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScreenAnim.generated.h"

/**
 * 
 */
UCLASS()
class WAS_API UScreenAnim : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintImplementableEvent)
	void DeathAnim();

	UFUNCTION(BlueprintImplementableEvent)
	void BeginPlayAnim();

	UFUNCTION(BlueprintImplementableEvent)
	void GameOverAnim();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayVideo1();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayVideo2();

	UFUNCTION(BlueprintImplementableEvent)
	void SkipVideo();
};
