#include "Door.h"
#include "Debug.h"
#include "Player/Scientist.h"
#include "Puzzles/BasePuzzle.h"

#include "Math/UnrealMathVectorCommon.h"
#include <Components/BoxComponent.h>
#include "GameHudWidget.h"

// Sets default values
ADoor::ADoor()
	: m_ShouldOpen(false)
	, OpenByRotate(false)
	, CanOpen(true)
	, EndDoor(false)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	SetRootComponent(DoorMesh);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionVolume"));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	TriggerBox->SetVisibility(false);


	DoorMoveDir = FVector(0.0f, 0.0f, 1.0f);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OverlapBeginn);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADoor::OverlapEnd);

	/*if (AttatchedPuzzle && !AttatchedPuzzle->IsSolved())
		TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);*/

	m_DoorLocation = DoorMesh->GetComponentLocation();
}

void ADoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!AttatchedPuzzle && TriggerBox->GetCollisionResponseToChannel(ECC_Pawn) != ECollisionResponse::ECR_Overlap)
	{
		TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}

	if (AttatchedPuzzle && AttatchedPuzzle->IsSolved() && TriggerBox->GetCollisionResponseToChannel(ECC_Pawn) == ECollisionResponse::ECR_Ignore)
	{
		TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}

	if (!OpenByRotate)
	{
		if (m_ShouldOpen && CanOpen)
			RaiseWall(DeltaSeconds);
		else
			LowerWall(DeltaSeconds);
	}
}

static FVector MoveDoorTowards(const FVector& current, const FVector& target, float maxDistanceDelta)
{
	// Distance
	float num = target.X - current.X;
	float num2 = target.Y - current.Y;
	float num3 = target.Z - current.Z;
	float num4 = num * num + num2 * num2 + num3 * num3;
	// Length of Distance
	float num5 = FMath::Sqrt(num4);

	if (num4 == 0.0f || (maxDistanceDelta >= 0.0f && num4 <= maxDistanceDelta * maxDistanceDelta))
		return target;

	return FVector
	(
		current.X + num / num5 * maxDistanceDelta,
		current.Y + num2 / num5 * maxDistanceDelta,
		current.Z + num3 / num5 * maxDistanceDelta
	);
}

void ADoor::RaiseWall(float deltaTime)
{
	// there is puzzle and it is not solved do not open
	if (AttatchedPuzzle != nullptr && !AttatchedPuzzle->IsSolved())
	{
		return;
	}

	FVector location = m_DoorLocation;

	if (DoorMoveDir.X == 1.0f)
	{
		location.X += DoorMoveDistance;
	}
	else if (DoorMoveDir.Z == 1.0f)
	{
		location.Z += DoorMoveDistance;
	}
	else
	{
		location.Y += DoorMoveDistance;
	}

	if (location == DoorMesh->GetComponentLocation())
		return;

	FVector newLocation = MoveDoorTowards(DoorMesh->GetComponentLocation(), location, deltaTime * OpenSpeed);
	DoorMesh->SetWorldLocation(newLocation);
}

void ADoor::LowerWall(float deltaTime)
{
	if (m_DoorLocation == DoorMesh->GetComponentLocation())
		return;

	FVector location = m_DoorLocation;
	FVector newLocation = MoveDoorTowards(DoorMesh->GetComponentLocation(), m_DoorLocation, deltaTime * OpenSpeed);

	DoorMesh->SetWorldLocation(newLocation);
}

void ADoor::OverlapBeginn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor
	, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AScientist* scientist = Cast<AScientist>(OtherActor);
	if (scientist)
	{
		if (!EndDoor)
		{
			m_ShouldOpen = true;
		}
		else
		{
			//m_ShouldOpen = scientist->GetInventory().Pills.Num() == 4;

			if (scientist->GetInventory().Pills.Num() == 4)
				scientist->EndGame();
		}

		GetWorldTimerManager().ClearTimer(m_AnimationTimer);

		if ((AttatchedPuzzle != nullptr && !AttatchedPuzzle->IsSolved()) || !CanOpen)
		{
			if (UGameHudWidget::Get())
				UGameHudWidget::Get()->AddHintText(FText::FromString(HintTextToShow));
		}

	}
}

void ADoor::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor
	, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (EndDoor)
		return;

	if (Cast<AScientist>(OtherActor))
	{
		if (UGameHudWidget::Get())
			UGameHudWidget::Get()->ClearHintText();

		auto lambda = [&]()
		{
			m_ShouldOpen = false;
			GetWorldTimerManager().ClearTimer(m_AnimationTimer);
		};

		GetWorldTimerManager().SetTimer(m_AnimationTimer, lambda, 2.0f, false);
	}
}
