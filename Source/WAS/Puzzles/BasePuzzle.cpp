// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/BasePuzzle.h"
#include "Scientist.h"
#include "Player/ScientistController.h"

#include "Tools/Debug.h"
#include "Tools/MyAssertTool.h"

#include <Camera/CameraComponent.h>
#include <Components/BoxComponent.h>


// Sets default values
ABasePuzzle::ABasePuzzle()
{
	PrimaryActorTick.bCanEverTick = false;

	m_Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionVolume"));
	SetRootComponent(m_Trigger);
	m_Trigger->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	m_Trigger->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	m_Trigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	m_Trigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	m_Trigger->SetVisibility(false);

	m_PuzzleCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PuzzleCamera"));
	m_PuzzleCamera->SetupAttachment(GetRootComponent());
	m_PuzzleCamera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	m_PuzzleCamera->bUsePawnControlRotation = true;
	m_PuzzleCamera->Activate(false);


	m_MainPlayerPtr = nullptr;
}

// Called when the game starts or when spawned
void ABasePuzzle::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePuzzle::Interact(AScientist* mainPlayer)
{
	ASSERT_BREAK(mainPlayer, "Player Pointer is null!!");
	m_MainPlayerPtr = mainPlayer;
}

void ABasePuzzle::Update()
{
	Debug::LogWarn("Not Implemented!");
}

bool ABasePuzzle::IsSolved()
{
	return false;
}

void ABasePuzzle::OnEnterPuzzle()
{
	Debug::LogWarn("Not Implemented!");
}

void ABasePuzzle::OnLeavePuzzle()
{
	Debug::LogWarn("Not Implemented!");
}

void ABasePuzzle::DestroyCamera()
{
	m_PuzzleCamera->DestroyComponent();
}

void ABasePuzzle::MouseCursor(bool active)
{
	if (m_MainPlayerPtr != nullptr)
	{
		if (active)
			m_MainPlayerPtr->SetUIInputOnly();
		else
			m_MainPlayerPtr->SetGameInputOnly();
	}
}

