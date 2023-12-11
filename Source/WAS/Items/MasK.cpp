// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Mask.h"
#include "GameHudWidget.h"
#include "Voicelines.h"
#include "Components/BoxComponent.h"
#include "Player/Scientist.h"

AMask::AMask()
{
	IsOnTheCharacter = false;
	OxygenMeter = 0.0f;
	MaxOxygenMeter = 100.0f;
	MinOxygenMeter = 0.0f;
	m_IsEmpty = true;

	CollisionBoxForInteraction = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxForInteraction"));
	CollisionBoxForInteraction->InitBoxExtent(FVector(5, 5, 5));
	RootComponent = CollisionBoxForInteraction;

	Mask = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mask"));
	Mask->SetRelativeLocation(FVector(10.0f, 5.0f, -5.0f));
	Mask->SetupAttachment(RootComponent);
}


void AMask::OxygenRefill()
{
	if (OxygenMeter < MaxOxygenMeter)
	{
		OxygenMeter = MaxOxygenMeter; 
		UGameHudWidget::Get()->SetOxygenBar(OxygenMeter);
		m_IsEmpty = false;
	}

	enum EEventType puzzleEvent = EEventType::OXYGEN;
	enum EEventProgress puzzleEventProgress = EEventProgress::ONGOING;
	AVoicelines::Get()->PlayVoiceLinesIfPossible(puzzleEvent, puzzleEventProgress, Priority::Mid); // Play voice lines for the refill event
}

void AMask::OxygenDepletion()
{
	// Decrease the oxygen meter over time
	if (OxygenMeter > MinOxygenMeter)
	{
		OxygenMeter -= DecressOxygenMeter; // Adjust the depletion rate as needed
		OxygenMeter = FMath::Clamp(OxygenMeter, MinOxygenMeter, MaxOxygenMeter);
		UGameHudWidget::Get()->SetOxygenBar(OxygenMeter);
	}

	// Check if the oxygen meter reaches zero, and if so, cause the character to die
	if (OxygenMeter == MinOxygenMeter)
	{
		enum EEventType puzzleEvent = EEventType::OXYGEN;
		enum EEventProgress puzzleEventProgress = EEventProgress::END;
		AVoicelines::Get()->PlayVoiceLinesIfPossible(puzzleEvent, puzzleEventProgress, Priority::Mid); // Play voice lines for the low oxygen event
		m_IsEmpty = true;
	}
}


void AMask::PickUP(AScientist* mainPlayer)
{
	if (mainPlayer == nullptr)
		return;

	ABaseItem::PickUP(mainPlayer);
	WearMask(mainPlayer);
}

void AMask::WearMask(AScientist* mainPlayer)
{
	if (mainPlayer)
	{
		IsOnTheCharacter = true;
		// Attach the mask to the character's mesh or socket
		//AttachToComponent(mainPlayer->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// Set the mask's location and rotation relative to the character
		//SetActorRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		//SetActorRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

		// Set the mask's visibility
		Mask->SetVisibility(true);

		// Update the mask's collision settings to disable interaction
		SetActorHiddenInGame(true);
		CollisionBoxForInteraction->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
