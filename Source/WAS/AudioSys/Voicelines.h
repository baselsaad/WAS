// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Voicelines.generated.h"

UENUM(BlueprintType)
enum class EEventProgress : uint8
{
	START, ONGOING, END, NUMBER
};

UENUM(BlueprintType)
enum class EEventType : uint8
{
	ORDERPUZZLE, BASEPUZZLE, CARPUZZLE, KEYPUZZLE, WALKING, OXYGEN, PILL
};

enum class Priority
{
	Low = 0, Mid = 1, High = 2
};

UCLASS()
class WAS_API AVoicelines : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoicelines();

	void PlayVoiceLines(EEventType event, EEventProgress eventProgress);
	void PlayVoiceLinesIfPossible(EEventType event, EEventProgress eventProgress, Priority priority);
	void PlayVoiceLinesNow(EEventType event, EEventProgress eventProgress, Priority priority);
	void PlayVoiceLinePill(const TCHAR* path);
	void StopAll();

	static AVoicelines* Get();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TMap<TPair<EEventType, EEventProgress>, int> voiceLineCounters;
	UAudioComponent* lastaudioComponent;
	TArray<FString> m_WasPlayedSounds;

	bool PlayVoiceLine(const TCHAR* voiceLine);
	
	//FString GetVoiceLinePath(EEventType event, EEventProgress eventProgress, int counter) const;

	UAudioComponent* m_AudioComp;
	Priority m_CurrentPlayedPriority;
};
