// Fill out your copyright notice in the Description page of Project Settings.


#include "CarPuzzle.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Tools/Debug.h"
#include "Player/Scientist.h"
#include "Player/ScientistController.h"
#include "Math/UnrealMathVectorCommon.h"
#include "GameHudWidget.h"
#include "Voicelines.h"


#include <Kismet/GameplayStatics.h>
#include <Components/BoxComponent.h>
#include <GameFramework/Actor.h>
#include <Components/ArrowComponent.h>
#include <DrawDebugHelpers.h>

ACarPuzzle::ACarPuzzle()
	: m_IsAttached(false)
	, m_IsInsideBox(false)
	, m_KeyIsPressed(false)
	, isMovingForword(true)
	, IsMovingSideway(true)

{
	PrimaryActorTick.bCanEverTick = true;

	m_Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	m_Trigger->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	m_Trigger->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	m_Trigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	m_Trigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	m_Trigger->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	m_Trigger->SetVisibility(true);

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	BoxMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxMesh->SetCollisionObjectType(ECC_WorldDynamic);
	BoxMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BoxMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	RootComponent = BoxMesh;

	m_Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	m_Arrow->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

void ACarPuzzle::BeginPlay()
{
	Super::BeginPlay();

	if (m_Trigger)
	{
		m_Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACarPuzzle::OverlapBeginn);
		m_Trigger->OnComponentEndOverlap.AddDynamic(this, &ACarPuzzle::OverlapEnd);
	}

	if (BoxMesh)
		BoxMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ACarPuzzle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_Player)
		return;

	if (m_Player->GetPlayerController()->WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		m_Player->ClearAllPlayerInputs();
		if (isMovingForword)
		{
			auto forward = [this](float val)
				{
					if (!m_IsInsideBox)
						return;

					if (!CheckCollision(m_Arrow->GetForwardVector() * val))
						m_Player->AddMovementInput(m_Arrow->GetForwardVector(), val);
				};

			FInputAxisBinding moveForward("MoveForward");
			moveForward.AxisDelegate.GetDelegateForManualSet().BindLambda(forward);
			m_Player->InputComponent->AxisBindings.Add(moveForward);
		}

		if (IsMovingSideway)
		{
			auto right = [this](float val)
				{
					if (!m_IsInsideBox)
						return;

					if (!CheckCollision(m_Arrow->GetRightVector() * val))
						m_Player->AddMovementInput(m_Arrow->GetRightVector(), val);
				};
			FInputAxisBinding moveRight("MoveRight");
			moveRight.AxisDelegate.GetDelegateForManualSet().BindLambda(right);
			m_Player->InputComponent->AxisBindings.Add(moveRight);

#
		}
		OnKeyPressed();

	}
	else if (m_Player->GetPlayerController()->WasInputKeyJustReleased(EKeys::LeftMouseButton))
	{
		m_Player->ClearAllPlayerInputs();
		m_Player->RestartAllPlayerInputs();
		OnKeyReleased();

	}
}

void ACarPuzzle::OverlapBeginn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	enum EEventType puzzleEvent = EEventType::CARPUZZLE;
	enum EEventProgress puzzleEventProgress = EEventProgress::START;
	AVoicelines::Get()->PlayVoiceLinesIfPossible(puzzleEvent, puzzleEventProgress, Priority::Mid); // Play voice lines for the start event

	m_Player = Cast<AScientist>(OtherActor);
	if (m_Player)
	{
		m_IsInsideBox = true;

		UGameHudWidget::Get()->ClearDialogText();
		UGameHudWidget::Get()->AddDialogText(FText::FromString("LMB"), FText::FromString("Halten"));

		if (isMovingForword)
		{
			UGameHudWidget::Get()->AddDialogText(FText::FromString("W,S"), FText::FromString(""));
		}

		if (IsMovingSideway)
		{
			UGameHudWidget::Get()->AddDialogText(FText::FromString("A,D"), FText::FromString(""));
		}
	}
}

void ACarPuzzle::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*m_Player = Cast<AScientist>(OtherActor);
	if (m_Player)
	{
		m_IsInsideBox = false;
		m_Player = nullptr;
		m_IsAttached = false;
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}*/

	m_Player = Cast<AScientist>(OtherActor);
	if (m_Player)
	{
		UGameHudWidget::Get()->ClearDialogText();
		m_IsInsideBox = false;
		m_Player = nullptr;
	}

	enum EEventType puzzleEvent = EEventType::CARPUZZLE;
	enum EEventProgress puzzleEventProgress = EEventProgress::END;
	AVoicelines::Get()->PlayVoiceLinesIfPossible(puzzleEvent, puzzleEventProgress, Priority::Mid); // Play voice lines for the ongoing event
}

// Called when the "LMB" key is pressed
void ACarPuzzle::OnKeyPressed()
{
	if (m_IsAttached)
		return;

	if (m_Player)
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, true);
		AttachToComponent(m_Player->GetMesh(), AttachmentRules);
		SetInstigator(m_Player);
		m_IsAttached = true;

		BoxMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}

	enum EEventType puzzleEvent = EEventType::CARPUZZLE;
	enum EEventProgress puzzleEventProgress = EEventProgress::ONGOING;
	AVoicelines::Get()->PlayVoiceLinesIfPossible(puzzleEvent, puzzleEventProgress, Priority::Mid); // Play voice lines for the ongoing event

}

void ACarPuzzle::OnKeyReleased()
{
	m_IsAttached = false;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	BoxMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

bool ACarPuzzle::CheckCollision(const FVector& Direction)
{
	FVector startLocation = GetActorLocation();
	startLocation.Z += 100.0f;
	FVector endLocation = startLocation + Direction * 30.0f;

	FBoxSphereBounds Bounds = BoxMesh->GetStaticMesh()->GetBoundingBox();
	FVector BoxExtent = Bounds.GetBox().GetExtent();

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	queryParams.AddIgnoredActor(m_Player);
	queryParams.AddIgnoredComponent(BoxMesh);

	FHitResult HitResult;
	bool hit = GetWorld()->SweepSingleByChannel(HitResult, startLocation, endLocation, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeBox(BoxExtent), queryParams);

	//DrawDebugBox(GetWorld(), endLocation, BoxExtent, FColor::Green, false, -1.0f, 0, 5.0f);


	/*if (hit)
		Debug::LogWarn("%s", *HitResult.Actor->GetName());*/

	return hit;
}