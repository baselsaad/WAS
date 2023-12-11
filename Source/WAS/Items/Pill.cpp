// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Pill.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Player/Scientist.h"
#include "Tools/Debug.h"
#include "LevelManager.h"
#include <Kismet/GameplayStatics.h>
#include "Voicelines.h"
#include "TimerManager.h"


APill::APill()
{

	CollisionBoxForInteraction = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxForInteraction"));
	CollisionBoxForInteraction->InitBoxExtent(FVector(5, 5, 5));
	RootComponent = CollisionBoxForInteraction;

	Pill = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pill"));
	Pill->SetRelativeLocation(FVector(10.0f, 5.0f, -5.0f));
	Pill->SetupAttachment(RootComponent);
}

void APill::PickUP(AScientist* mainPlayer)
{
	if (mainPlayer == nullptr)
		return;

	ABaseItem::PickUP(mainPlayer);
	
	SetActorHiddenInGame(true);
	{
		mainPlayer->IncreamntCurrnetRun();
		mainPlayer->GetInventory().Pills.AddUnique(UniqueID);
		
		// Reload the game
		GetWorld()->GetGameInstanceChecked<ULevelManager>()->SaveGame(mainPlayer);
		GetWorld()->GetGameInstanceChecked<ULevelManager>()->LoadGame();
	}

	CollisionBoxForInteraction->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Onpickup(mainPlayer);
}

void APill::Onpickup(AScientist* mainPlayer)
{
	int pill_number = mainPlayer->GetInventory().Pills.Num();

	if (pill_number == 1)
	{
		mainPlayer->PlayFirstMemVideo();
		return;
	}

	if (pill_number == 2)
	{
		AVoicelines::Get()->PlayVoiceLinePill(ANSI_TO_TCHAR("/Game/Sound/pill_ongoing_1_Cue"));

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, [this]()
		{
			AVoicelines::Get()->PlayVoiceLinePill(ANSI_TO_TCHAR("/Game/Sound/pill_start_3_Cue"));
		}
		, 55.0f, false);
	}

	if (pill_number == 3)
	{
		mainPlayer->PlaySecMemVideo();
		return;
	}

	if (pill_number == 4)
	{
		AVoicelines::Get()->PlayVoiceLinePill(ANSI_TO_TCHAR("/Game/Sound/pill_start_4_Cue"));
	}

	//enum EEventType puzzleEvent = EEventType::PILL;

	//if (pill_number >= 3)
	//{
	//	enum EEventProgress puzzleEventProgress = EEventProgress::ONGOING;
	//	AVoicelines::Get()->PlayVoiceLinesNow(puzzleEvent, puzzleEventProgress, Priority::High);
	//	
	//	//delay 
	//	mainPlayer->OnStartMemory();

	//	FTimerHandle TimerHandle;
	//	GetWorldTimerManager().SetTimer(TimerHandle, [this]() 
	//	{
	//		AScientist* player = Cast<AScientist>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	//		PlayDelayedVoiceLine(player);
	//	}
	//	, 55.0f, false);

	//}
	//else {
	//	//play memory video here

	//	enum EEventProgress puzzleEventProgress = EEventProgress::START;
	//	AVoicelines::Get()->PlayVoiceLinesNow(puzzleEvent, puzzleEventProgress, Priority::High); // Play voice lines after the memory video
	//}

}

void APill::PlayDelayedVoiceLine(AScientist* mainPlayer)
{
	// Play voice lines after the delay
	enum EEventType puzzleEvent = EEventType::PILL;
	enum EEventProgress puzzleEventProgress = EEventProgress::START;
	AVoicelines::Get()->PlayVoiceLinesNow(puzzleEvent, puzzleEventProgress, Priority::High);
	mainPlayer->OnStopMemory();
}
