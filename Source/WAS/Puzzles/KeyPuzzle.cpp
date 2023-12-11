#include "KeyPuzzle.h"
#include "Player/Scientist.h"
#include "Player/ScientistController.h"
#include "Tools/Debug.h"
#include "Tools/MyAssertTool.h"
#include "GameHudWidget.h"
#include "Math/UnrealMathVectorCommon.h"
#include "Voicelines.h"

#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Engine/World.h>
#include <Components/BoxComponent.h>
#include "Engine/PostProcessVolume.h"
#include <Components/CapsuleComponent.h>


/*
*		1			0
*			  2
*		5			3
*			6    4
*
*/

//leider hard coded :)
static int s_CurrentHighLighted[3][3] =
{
	{1, 2, 0},
	{3, 2, 4},
	{6, 2, 5}
};
static int s_CurrentHighLightedIndex = 0;
TArray<FVector> s_CopiedPositions;

void AKeyPuzzle::BeginPlay()
{
	Super::BeginPlay();

	s_CopiedPositions = m_PieceHolder;
	m_RandomPositon = { 1,5,0,2,3,6,4 };

	if (!m_Material)
	{
		Debug::LogErr("Can not find the Material");
	}

	m_DynamicMaterial = UMaterialInstanceDynamic::Create(m_Material, this);
	if (m_DynamicMaterial)
	{
		m_DynamicMaterial->SetScalarParameterValue(TEXT("Glow Intensity"), 25.0f);

		if (m_PostProcessingVolume)
		{
			m_PostProcessingVolume->Settings.WeightedBlendables.Array.Empty();
			FWeightedBlendable NewBlendable;
			NewBlendable.Object = m_DynamicMaterial;
			NewBlendable.Weight = 1.0f;
			m_PostProcessingVolume->Settings.WeightedBlendables.Array.Add(NewBlendable);
			m_PostProcessingVolume->Settings.WeightedBlendables.Array.Shrink();
		}
	}

	FTransform transform;
	m_CosutomPivot = GetWorld()->SpawnActorDeferred<ACharacter>(ACharacter::StaticClass(), transform);
	if (m_CosutomPivot)
	{
		m_CosutomPivot->SetActorHiddenInGame(true);
		m_CosutomPivot->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		m_CosutomPivot->SetActorLocation(FVector::ZeroVector);
		m_CosutomPivot->SetActorRotation(FRotator::ZeroRotator);
		m_CosutomPivot->FinishSpawning(transform);
	}
}

AKeyPuzzle::AKeyPuzzle()
	: Solved(false)
{
	PrimaryActorTick.bCanEverTick = true;


	AnimRotateDir = FRotator(0.0f, 0.0f, -120.0f);
	AnimRotationRate = 10.0f;
}

void AKeyPuzzle::Interact(AScientist* mainPlayer)
{
	Super::Interact(mainPlayer);

	if (Solved)
		return;

	OnEnterPuzzle();

	if (m_CollectedPieces.Num() != m_Pieces.Num())
	{
		UGameHudWidget::Get()->ClearDialogText();
		UGameHudWidget::Get()->AddDialogText(FText::FromString("Space"), FText::FromString("Schluessel abgeben"));
		UGameHudWidget::Get()->AddDialogText(FText::FromString("Q"), FText::FromString("Verlassen"));
	}
	else
	{
		UGameHudWidget::Get()->ClearDialogText();
		UGameHudWidget::Get()->AddDialogText(FText::FromString("Space"), FText::FromString("Rotation"));
		UGameHudWidget::Get()->AddDialogText(FText::FromString("A"), FText::FromString("Waehlen"));
		UGameHudWidget::Get()->AddDialogText(FText::FromString("D"), FText::FromString("Waehlen"));
		HighLightSelectedPieces();
	}
}

void AKeyPuzzle::Update()
{
}

void AKeyPuzzle::OnEnterPuzzle()
{
	if (m_MainPlayerPtr)
	{
		enum EEventType puzzleEvent = EEventType::KEYPUZZLE;
		enum EEventProgress puzzleEventProgress = EEventProgress::START;
		AVoicelines::Get()->PlayVoiceLinesIfPossible(puzzleEvent, puzzleEventProgress, Priority::Mid); // Play voice lines for the start event

		m_ShouldAnimate = true;
		m_MainPlayerPtr->SwitchPlayerCamera(m_PuzzleCamera);
		m_MainPlayerPtr->ClearAllPlayerInputs();
		m_MainPlayerPtr->InputComponent->BindAction("LeavePuzzle", EInputEvent::IE_Pressed, this, &AKeyPuzzle::LeavePuzzle); // Q
	}
}

void AKeyPuzzle::OnLeavePuzzle()
{
	UGameHudWidget::Get()->ClearDialogText();

	if (m_MainPlayerPtr)
	{
		m_MainPlayerPtr->ClearAllPlayerInputs();
		m_MainPlayerPtr->RestartAllPlayerInputs();
		m_MainPlayerPtr->SwitchPlayerCamera(nullptr);
		m_MainPlayerPtr->OnLeavePuzzle();
		m_MainPlayerPtr.Reset();
		m_MainPlayerPtr = nullptr;
	}

	for (APiece* p : m_Pieces)
	{
		p->Mesh->SetRenderCustomDepth(false);
		//p->Mesh->SetCustomDepthStencilValue(0);
		if (!Solved && m_CollectedPieces.Num() == m_Pieces.Num())
			p->SetActorLocation(s_CopiedPositions[m_RandomPositon[p->UniqeKey]]);
	}

	m_ShouldAnimate = false;
}

void AKeyPuzzle::OnGetKeyFromPlayer(APiece* piece, bool load)
{
	if (piece)
	{
		piece->SetActorRotation(piece->m_OrginalRotation);
	}

	m_CollectedPieces.AddUnique(piece);

	// Instead of loading and random postion Get the correct position from m_RandomPositon based on the unique key

	if (piece->CurrentIndex == -1)
	{
		int index = m_RandomPositon[piece->UniqeKey];
		if (index < m_PieceHolder.Num())
		{
			piece->SetActorLocation(m_PieceHolder[index]);

			for (int i = 0; i < s_CopiedPositions.Num(); i++)
				if (s_CopiedPositions[i] == m_PieceHolder[index])
					piece->CurrentIndex = i;

			piece->SetActorHiddenInGame(false);
		}
	}

	if (load)
	{
		if (Solved) // this will get called when loading the game 
		{
			piece->SetActorLocation(s_CopiedPositions[piece->UniqeKey]);
			piece->SetActorHiddenInGame(false);
			piece->DestroyTriggerBox();
			return;
		}
		else
		{
			int index = m_RandomPositon[piece->UniqeKey];
			piece->SetActorLocation(s_CopiedPositions[index]);
			piece->SetActorHiddenInGame(false);
			piece->DestroyTriggerBox();
			return;
		}
	}


	if (m_CollectedPieces.Num() == m_Pieces.Num())
	{
		UGameHudWidget::Get()->ClearDialogText();
		UGameHudWidget::Get()->AddDialogText(FText::FromString("Space"), FText::FromString("Rotation"));
		UGameHudWidget::Get()->AddDialogText(FText::FromString("A"), FText::FromString("Waehlen"));
		UGameHudWidget::Get()->AddDialogText(FText::FromString("D"), FText::FromString("Waehlen"));

		for (int i = 0; i < 3; i++)
		{
			int index1 = s_CurrentHighLighted[i][0];
			int index2 = s_CurrentHighLighted[i][1];
			int index3 = s_CurrentHighLighted[i][2];

			auto& l1 = s_CopiedPositions[index1];
			auto& l2 = s_CopiedPositions[index2];
			auto& l3 = s_CopiedPositions[index3];

			for (APiece* ptr : m_Pieces)
			{
				if (ptr->GetActorLocation() == l1)
					ptr->CurrentIndex = index1;
				else if (ptr->GetActorLocation() == l2)
					ptr->CurrentIndex = index2;
				else if (ptr->GetActorLocation() == l3)
					ptr->CurrentIndex = index3;
			}
		}

		HighLightSelectedPieces();
	}
}

void AKeyPuzzle::LeavePuzzle()
{
	if (!GetWorldTimerManager().IsTimerActive(m_RotationTimerHandle))
		OnLeavePuzzle();
}

void AKeyPuzzle::RotateAnimation()
{
	FVector averagePosition;

	APiece* p1 = nullptr;
	APiece* p2 = nullptr;
	APiece* p3 = nullptr;

	int index1 = s_CurrentHighLighted[s_CurrentHighLightedIndex][0];
	int index2 = s_CurrentHighLighted[s_CurrentHighLightedIndex][1];
	int index3 = s_CurrentHighLighted[s_CurrentHighLightedIndex][2];

	FVector v1 = s_CopiedPositions[index1];
	FVector v2 = s_CopiedPositions[index2];
	FVector v3 = s_CopiedPositions[index3];

	averagePosition = (v1 + v2 + v3) / 3;
	m_CosutomPivot->SetActorLocation(averagePosition);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, false);

	for (int i = 0; i < 3; i++)
	{
		int index = s_CurrentHighLighted[s_CurrentHighLightedIndex][i];

		const FVector& position = s_CopiedPositions[index];
		for (auto* p : m_Pieces)
		{
			if (p->GetActorLocation().Equals(position, 0.01f))
			{
				p->AttachToComponent(m_CosutomPivot->GetRootComponent(), AttachmentRules);
				//p->Mesh->SetRenderCustomDepth(false);
				switch (i)
				{
					case 0: p1 = p; break;
					case 1: p2 = p; break;
					case 2: p3 = p; break;
				}
			}
		}
	}

	static bool isMoving = true;
	static bool bMovingForward = true;
	static bool bShouldRotate = false;
	static float elapsed = 0.0f;

	elapsed = 0.0f;
	isMoving = true;
	bShouldRotate = false;
	bMovingForward = true;

	FVector InitialPosition = m_CosutomPivot->GetActorLocation();

	const FRotator TargetRotation = AnimRotateDir;

	GetWorldTimerManager().ClearTimer(m_RotationTimerHandle);
	GetWorldTimerManager().SetTimer(m_RotationTimerHandle, [this, TargetRotation, p1, p2, p3, v1, v2, v3, index1, index2, index3, InitialPosition]()
	{
		const float MovementDistance = 10.0f;
		const float MovementSpeed = 30.0f;

		FVector TargetPosition;

		if (bMovingForward)
		{
			TargetPosition = InitialPosition + (m_CosutomPivot->GetActorForwardVector() * MovementDistance) * 1.0f;
		}
		else
		{
			TargetPosition = InitialPosition;
		}


		if (!bShouldRotate)
		{
			float CurrentMovementDistance = FVector::Distance(m_CosutomPivot->GetActorLocation(), InitialPosition);

			if (isMoving)
			{
				float MovementDelta = MovementSpeed * GetWorld()->GetDeltaSeconds();
				FVector NewPosition = FMath::VInterpTo(m_CosutomPivot->GetActorLocation(), TargetPosition, GetWorld()->GetDeltaSeconds(), MovementSpeed);
				m_CosutomPivot->SetActorLocation(NewPosition);
			}

			if (CurrentMovementDistance >= MovementDistance)
			{
				elapsed += GetWorld()->DeltaTimeSeconds;
				isMoving = false;

				if (elapsed >= 0.06f)
					bShouldRotate = true;
			}
		}

		if (bShouldRotate && !isMoving)
		{
			// Rotate
			const FRotator InterpolatedRotation = FMath::RInterpTo(m_CosutomPivot->GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), AnimRotationRate);
			m_CosutomPivot->SetActorRotation(InterpolatedRotation);
			p1->SetActorRotation(p1->m_OrginalRotation);
			p2->SetActorRotation(p1->m_OrginalRotation);
			p3->SetActorRotation(p1->m_OrginalRotation);

			if (m_CosutomPivot->GetActorRotation().Equals(AnimRotateDir, 0.01f))
			{
				// Animation complete, detach the pieces and update their positions
				for (auto* p : m_Pieces)
				{
					p->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				}

				if (p1 && p2 && p3)
				{
					p1->SetActorLocation(v3);
					p2->SetActorLocation(v1);
					p3->SetActorLocation(v2);

					p1->CurrentIndex = index3;
					p2->CurrentIndex = index1;
					p3->CurrentIndex = index2;

					/*p1->Mesh->SetRenderCustomDepth(true);
					p2->Mesh->SetRenderCustomDepth(true);
					p3->Mesh->SetRenderCustomDepth(true);*/
				}
				m_CosutomPivot->SetActorRotation(FRotator::ZeroRotator);


				bMovingForward = false;
				bShouldRotate = false;
				isMoving = false;
			}

			if (!bShouldRotate && !isMoving)
			{
				if (IsSolved())
					OnLeavePuzzle();

				GetWorldTimerManager().ClearTimer(m_RotationTimerHandle);
			}
		}
	}
	, GetWorld()->DeltaTimeSeconds, true);
}

void AKeyPuzzle::HighLightSelectedPieces()
{
	// for this effect to work
	// the scene should have postprocessing volume and post processing material should have the outline effect material 
	// also make sure that the pieces should be in postprocessing volume or the postprocessing volume should have infinte bound
	if (m_CollectedPieces.Num() != m_Pieces.Num())
		return;

	for (APiece* p : m_Pieces)
	{
		p->Mesh->SetRenderCustomDepth(false);
		//p->Mesh->SetCustomDepthStencilValue(0);
	}

	for (int i = 0; i < 3; i++)
	{
		int index = s_CurrentHighLighted[s_CurrentHighLightedIndex][i];
		//Debug::LogWarn("Index: %d", index);

		const FVector& position = s_CopiedPositions[index];
		for (auto* p : m_Pieces)
		{
			if (p->GetActorLocation().Equals(position, 0.01f))
			{
				p->Mesh->SetRenderCustomDepth(true);
			}
		}
	}
}

void AKeyPuzzle::SwitchSelectedPieces(int direction)
{
	s_CurrentHighLightedIndex += direction + 3;
	s_CurrentHighLightedIndex %= 3;

	HighLightSelectedPieces();
}

void AKeyPuzzle::AnimateHighLightedPiece(float DeltaSeconds)
{
	if (m_DynamicMaterial)
	{
		static float dir = 1.0f;

		float outLineWidth = 0.0f;
		if (!m_DynamicMaterial->GetScalarParameterValue(TEXT("Line Width"), outLineWidth))
		{
			Debug::LogErr("Could not find the parameter Line Width");
			return;
		}

		if (outLineWidth >= 2.0f || outLineWidth <= 0.2f)
		{
			dir *= -1;
		}

		outLineWidth += (dir * DeltaSeconds * AnimationSpeedRate);
		m_DynamicMaterial->SetScalarParameterValue(TEXT("Line Width"), outLineWidth);
	}
}

void AKeyPuzzle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (m_MainPlayerPtr && !GetWorldTimerManager().IsTimerActive(m_RotationTimerHandle))
	{
		// collected pieces not complete
		if (m_CollectedPieces.Num() < m_Pieces.Num())
		{
			if (m_MainPlayerPtr->GetPlayerController()->WasInputKeyJustPressed(EKeys::SpaceBar))
				m_MainPlayerPtr->GetInventory().GiveCollectedKeyPiceeUp(*this);
		}
		else if (m_CollectedPieces.Num() == m_Pieces.Num()) // Collected pieces completed
		{
			if (m_MainPlayerPtr->GetPlayerController()->WasInputKeyJustPressed(EKeys::A))
			{
				SwitchSelectedPieces(-1);
			}

			if (m_MainPlayerPtr->GetPlayerController()->WasInputKeyJustPressed(EKeys::D))
			{
				SwitchSelectedPieces(1);
			}

			if (m_MainPlayerPtr->GetPlayerController()->WasInputKeyJustPressed(EKeys::SpaceBar))
			{
				RotateAnimation();
			}
		}
	}


	if (m_ShouldAnimate)
		AnimateHighLightedPiece(DeltaSeconds);
}

bool AKeyPuzzle::IsSolved()
{
	for (auto piece : m_Pieces)
	{
		if (piece->CurrentIndex != piece->UniqeKey)
		{
			Solved = false;
			return false;
		}
	}


	Solved = true;
	return Solved;
}

APiece::APiece()
	: CurrentIndex(-1)
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(TriggerBox);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionVolume"));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	TriggerBox->SetVisibility(false);


	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

void APiece::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (m_MainPlayer && CurrentIndex == -1)
	{
		if (m_MainPlayer->GetPlayerController()->WasInputKeyJustPressed(EKeys::E))
		{
			m_MainPlayer->GetInventory().CollectKey(this);
		}
	}
}

void APiece::DestroyTriggerBox()
{
	if (TriggerBox)
	{
		TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
}

void APiece::OverlapBeginn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	m_MainPlayer = Cast<AScientist>(OtherActor);
	if (m_MainPlayer && CurrentIndex == -1)
	{
		UGameHudWidget::Get()->AddDialogText(FText::FromString("E"), FText::FromString("Nehmen"));
	}
}

void APiece::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AScientist>(OtherActor))
	{
		UGameHudWidget::Get()->ClearDialogText();
		m_MainPlayer.Reset();
		m_MainPlayer = nullptr;
	}
}

void APiece::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APiece::OverlapBeginn);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APiece::OverlapEnd);
	m_OrginalRotation = FRotator(0.0f, 270.0f, 90.0);
}
