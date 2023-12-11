// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzles/OrderPuzzle.h"
#include "WAS.h"
#include "Player/Scientist.h"
#include "Player/ScientistController.h"
#include "Tools/Debug.h"
#include "Tools/MyAssertTool.h"
#include "Menu/GameHudWidget.h"
#include "Voicelines.h"
#include "Items/Article.h"

#include <Kismet/GameplayStatics.h>
#include <Engine/World.h>
#include <Engine/PostProcessVolume.h>
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraActor.h"
#include "Engine/PostProcessVolume.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/CameraComponent.h"



AOrderPuzzle::AOrderPuzzle()
	: m_CurrentOpendPaper(nullptr)
	, MinFOV(90.0f)
	, MaxFOV(120.0f)
	, ZoomSpeed(10.0f)
	, m_WasSolved(false)
{
	PrimaryActorTick.bCanEverTick = true;
	m_LerpSpeed = 250.0f;
}

void AOrderPuzzle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (m_MainPlayerPtr == nullptr)
		return;

	if (m_CurrentOpendPaper)
	{
		if (m_CurrentOpendPaper->PaperTyp == PaperType::NOTE)
		{
			m_CurrentOpendPaper->RotateWithMouse(m_MainPlayerPtr->GetPlayerController());
			m_CurrentOpendPaper->RotateWithDAndA(m_MainPlayerPtr->GetPlayerController());
		}
		else
		{
			m_CurrentOpendPaper->MoveWithMouseWithinRange(m_MainPlayerPtr->GetPlayerController(), m_CurrentOpendPaper->GetActorLocation(), m_CurrentOpendPaper->GetActorLocation() + 10.0f);
		}
	}
}

void AOrderPuzzle::Interact(AScientist* mainPlayer)
{
	ABasePuzzle::Interact(mainPlayer);

	OnEnterPuzzle();
	MouseCursor(true);

	UGameHudWidget::Get()->ClearDialogText();
	UGameHudWidget::Get()->AddDialogText(FText::FromString("A"), FText::FromString("Tausche die rechten Zettel"), 5);
	UGameHudWidget::Get()->AddDialogText(FText::FromString("D"), FText::FromString("Tausche die linken Zettel"), 5);
	UGameHudWidget::Get()->AddDialogText(FText::FromString("Q"), FText::FromString("Verlassen"), 5);

	for (auto article : m_Articles)
	{
		if (!mainPlayer->GetInventory().CollectedArticles.Contains(article))
			mainPlayer->GetInventory().CollectArticle(article);
	}
}

void AOrderPuzzle::Update()
{
}

bool AOrderPuzzle::IsSolved()
{
	if (m_WasSolved)// if not check
		return true;

	for (int i = 0; i < m_Notes.Num(); i++)
	{
		if (i != m_Notes[i]->CorrectIndex)
		{
			// the puzzle is not solved 
			enum EEventType puzzleEvent = EEventType::ORDERPUZZLE;
			enum EEventProgress puzzleEventProgress = EEventProgress::ONGOING;
			AVoicelines::Get()->PlayVoiceLinesIfPossible(puzzleEvent, puzzleEventProgress, Priority::Mid); // Play voice lines for the ongoing event
			return false;
		}
	}

	Debug::LogWarn("Solved");
	// Flip notes if Solved
	m_WasSolved = true;
	for (int i = 0; i < m_Notes.Num(); i++)
	{
		auto rotation = m_Notes[i]->GetActorRotation();
		m_Notes[i]->SetActorRotation(FRotator(rotation.Pitch, rotation.Yaw + 180.0f, rotation.Roll));
	}


	enum EEventType puzzleEvent = EEventType::ORDERPUZZLE;
	enum EEventProgress puzzleEventProgress = EEventProgress::END;
	AVoicelines::Get()->PlayVoiceLinesIfPossible(puzzleEvent, puzzleEventProgress, Priority::Mid); // Play voice lines for the end event
	// Play Sound that the puzzle is solved 
	return true;
}

void AOrderPuzzle::OnEnterPuzzle()
{
	if (m_MainPlayerPtr == nullptr)
		return;

	m_MainPlayerPtr->SwitchPlayerCamera(m_PuzzleCamera);
	m_MainPlayerPtr->ClearAllPlayerInputs();

	m_MainPlayerPtr->InputComponent->BindAction("LeavePuzzle", EInputEvent::IE_Pressed, this, &AOrderPuzzle::LeavePuzzle); // Q
	m_MainPlayerPtr->InputComponent->BindAction("LeftMouseClick", EInputEvent::IE_Pressed, this, &AOrderPuzzle::OnLeftMouseClicked);
	m_MainPlayerPtr->InputComponent->BindAction("RightMouseClick", EInputEvent::IE_Pressed, this, &AOrderPuzzle::OnRightMouseClicked);

	m_MainPlayerPtr->InputComponent->BindAction("ASwapNote", EInputEvent::IE_Pressed, this, &AOrderPuzzle::SwapNoteWithA);
	m_MainPlayerPtr->InputComponent->BindAction("DSwapNote", EInputEvent::IE_Pressed, this, &AOrderPuzzle::SwapNoteWithD);
	m_MainPlayerPtr->InputComponent->BindAxis("MouseWheel", this, &AOrderPuzzle::HandleZoomEvent);

	enum EEventType puzzleEvent = EEventType::ORDERPUZZLE;
	enum EEventProgress puzzleEventProgress = EEventProgress::START;
	AVoicelines::Get()->PlayVoiceLinesIfPossible(puzzleEvent, puzzleEventProgress, Priority::Mid); // Play voice lines for the start event
}

void AOrderPuzzle::OnLeavePuzzle()
{
	if (m_MainPlayerPtr == nullptr)
		return;

	m_MainPlayerPtr->ClearAllPlayerInputs();
	m_MainPlayerPtr->RestartAllPlayerInputs();
	m_MainPlayerPtr->SwitchPlayerCamera(nullptr);
	m_MainPlayerPtr->GetPlayerController()->DisableDepthOfField(m_PuzzleCamera); // disable depth of field in case player leave while open paper 
	MouseCursor(false);

	if (m_CurrentOpendPaper)
	{
		m_CurrentOpendPaper->ClosePaper();
		m_CurrentOpendPaper = nullptr;
	}

	m_MainPlayerPtr->OnLeavePuzzle();
	m_MainPlayerPtr.Reset();
	m_MainPlayerPtr = nullptr;
}

void AOrderPuzzle::SolvePuzzle()
{
	// First Copy Locations
	for (int i = 0; i < m_Notes.Num(); i++)
	{
		int index = m_Notes[i]->CorrectIndex;
		m_Notes[i]->SetActorLocation(m_Notes[index]->GetOrginalLocation());
		auto rotation = m_Notes[i]->GetActorRotation();
		m_Notes[i]->SetActorRotation(FRotator(rotation.Pitch, rotation.Yaw + 180.0f, rotation.Roll));
	}

	/*for (int i = 0; i < m_Notes.Num(); i++)
	{
		int CorrectIndex = m_Notes[i]->CorrectIndex;
		m_Notes.Swap(i, CorrectIndex);
	}*/

	m_WasSolved = true;
}


static FVector MoveTowards(const FVector& current, const FVector& target, float maxDistanceDelta)
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

void AOrderPuzzle::SwapNoteWithA()
{
	if (m_CurrentOpendPaper || GetWorldTimerManager().IsTimerActive(PaperAnimationTimer))
		return;

	APaper* SecondPaper = m_Notes[1];
	APaper* LastPaper = m_Notes[2];

	if (!SecondPaper || !LastPaper)
		return;

	FVector SecondPaperStartPos = SecondPaper->GetOrginalLocation();
	FVector LastPaperStartPos = LastPaper->GetOrginalLocation();

	FVector SecondPaperEndPos = LastPaperStartPos;
	FVector LastPaperEndPos = SecondPaperStartPos;

	auto AKeyPressed = [this, SecondPaperStartPos, LastPaper, SecondPaper, LastPaperEndPos, SecondPaperEndPos, LastPaperStartPos]() -> void
	{
		FVector NewLastPosEnd = MoveTowards(LastPaper->GetActorLocation(), LastPaperEndPos, GetWorld()->DeltaTimeSeconds * m_LerpSpeed);
		FVector NewSecondPaperPos = MoveTowards(SecondPaper->GetActorLocation(), SecondPaperEndPos, GetWorld()->DeltaTimeSeconds * m_LerpSpeed);

		LastPaper->SetActorLocation(NewLastPosEnd);
		SecondPaper->SetActorLocation(NewSecondPaperPos);

		if (NewLastPosEnd == LastPaperEndPos && NewSecondPaperPos == SecondPaperEndPos)
		{
			if (IsSolved())
				OnLeavePuzzle();

			GetWorldTimerManager().ClearTimer(PaperAnimationTimer);
		}
	};

	GetWorldTimerManager().ClearTimer(PaperAnimationTimer);
	GetWorldTimerManager().SetTimer(PaperAnimationTimer, AKeyPressed, GetWorld()->DeltaTimeSeconds, true);

	LastPaper->SetNewOrginalPosition(LastPaperEndPos);
	SecondPaper->SetNewOrginalPosition(SecondPaperEndPos);
	m_Notes.Swap(1, 2);


	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PostItRemove, GetActorLocation());
}

void AOrderPuzzle::SwapNoteWithD()
{
	if (m_CurrentOpendPaper || GetWorldTimerManager().IsTimerActive(PaperAnimationTimer))
		return;

	APaper* FirstPaper = m_Notes[0];
	APaper* SecondPaper = m_Notes[1];

	if (!FirstPaper || !SecondPaper)
		return;

	FVector FirstPaperStartPos = FirstPaper->GetOrginalLocation();
	FVector SecondPaperStartPos = SecondPaper->GetOrginalLocation();

	FVector FirstPaperEndPos = SecondPaperStartPos;
	FVector SecondPaperEndPos = FirstPaperStartPos;

	auto DKeyPressed = [this, SecondPaperStartPos, FirstPaperStartPos, FirstPaper, SecondPaper, FirstPaperEndPos, SecondPaperEndPos]() -> void
	{
		FVector NewFirstEndPos = MoveTowards(FirstPaper->GetActorLocation(), FirstPaperEndPos, GetWorld()->DeltaTimeSeconds * m_LerpSpeed);
		FVector NewSecondPaperPos = MoveTowards(SecondPaper->GetActorLocation(), SecondPaperEndPos, GetWorld()->DeltaTimeSeconds * m_LerpSpeed);

		FirstPaper->SetActorLocation(NewFirstEndPos);
		SecondPaper->SetActorLocation(NewSecondPaperPos);

		if (NewFirstEndPos == FirstPaperEndPos && NewSecondPaperPos == SecondPaperEndPos)
		{
			if (IsSolved())
				OnLeavePuzzle();

			GetWorldTimerManager().ClearTimer(PaperAnimationTimer);
		}
	};

	GetWorldTimerManager().ClearTimer(PaperAnimationTimer);
	GetWorldTimerManager().SetTimer(PaperAnimationTimer, DKeyPressed, GetWorld()->DeltaTimeSeconds, true);

	FirstPaper->SetNewOrginalPosition(FirstPaperEndPos);
	SecondPaper->SetNewOrginalPosition(SecondPaperEndPos);

	m_Notes.Swap(0, 1);
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PostItRemove, GetActorLocation());
}

void AOrderPuzzle::HandleZoomEvent(float value)
{
	if (m_PuzzleCamera == nullptr)
		return;

	float NewFOV = FMath::Clamp(m_PuzzleCamera->FieldOfView + (-1 * value) * ZoomSpeed, MinFOV, MaxFOV);
	m_PuzzleCamera->SetFieldOfView(NewFOV);
}

void AOrderPuzzle::LeavePuzzle()
{
	OnLeavePuzzle();
	GetWorldTimerManager().ClearTimer(PaperAnimationTimer);
}

void AOrderPuzzle::OnLeftMouseClicked()
{
	if (m_CurrentOpendPaper != nullptr || m_MainPlayerPtr == nullptr) // then we already open a paper
		return;

	FHitResult HitResult;
	m_MainPlayerPtr->GetPlayerController()->GetHitResultUnderCursor(MOUSE_PAPER_TRACE_CHANNEL, false, HitResult);

	// Check if an actor was hit
	if (HitResult.Actor.IsValid())
	{
		Debug::LogWarn("Name %s", *HitResult.Actor.Get()->GetName());
		APaper* paper = Cast<APaper>(HitResult.Actor.Get());
		if (paper)
		{
			m_CurrentOpendPaper = paper;
			OpenPaper();
		}
	}
}

void AOrderPuzzle::OnRightMouseClicked()
{
	if (m_CurrentOpendPaper)
	{
		m_MainPlayerPtr->GetPlayerController()->DisableDepthOfField(m_PuzzleCamera);
		m_CurrentOpendPaper->ClosePaper();
		m_CurrentOpendPaper = nullptr;
	}
}

void AOrderPuzzle::OpenPaper()
{
	if (m_CurrentOpendPaper == nullptr)
		return;

	FVector cameraLocation;
	FRotator cameraRotation;
	m_MainPlayerPtr->GetPlayerController()->GetPlayerViewPoint(cameraLocation, cameraRotation);

	auto location = cameraLocation + cameraRotation.Vector() * m_DistanceFromCamera;
	m_CurrentOpendPaper->m_OpenLocation = location;
	m_CurrentOpendPaper->SetActorLocation(m_CurrentOpendPaper->m_OpenLocation);

	m_CurrentOpendPaper->m_MaxMovementPos.X = location.X + 10.0f;
	m_CurrentOpendPaper->m_MaxMovementPos.Y = location.Z + 10.0f;

	m_CurrentOpendPaper->m_MinMovementPos.X = location.X - 10.0f;
	m_CurrentOpendPaper->m_MinMovementPos.Y = location.Z - 10.0f;

	FRotator PaperRotation = (cameraLocation - m_CurrentOpendPaper->GetActorLocation()).Rotation();

	if (m_CurrentOpendPaper->PaperTyp == PaperType::PAPER)
		PaperRotation.Pitch -= 90.0f;

	m_CurrentOpendPaper->SetActorRotation(PaperRotation);

	float distance = FVector::Dist(m_CurrentOpendPaper->GetActorLocation(), cameraLocation);
	m_MainPlayerPtr->GetPlayerController()->EnableDepthOfFieldOnObject(m_PuzzleCamera, distance);

	if (m_CurrentOpendPaper->PaperTyp == PaperType::PAPER)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Paper, GetActorLocation());
	}
	else
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PostItRemove2, GetActorLocation());
	}

}

void AOrderPuzzle::BeginPlay()
{
	Super::BeginPlay();
}

void AOrderPuzzle::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(PaperAnimationTimer);
}

APaper::APaper()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	CorrectIndex = NOT_NOTE;
}

void APaper::ClosePaper()
{
	SetActorLocation(m_OrginalLocation);
	SetActorRotation(m_OrginalRotation);
}

void APaper::RotateWithMouse(APlayerController* playerController)
{
	if (playerController == nullptr)
		return;

	float mouseX, mouseY;
	playerController->GetInputMouseDelta(mouseX, mouseY);

	if (playerController->IsInputKeyDown(EKeys::LeftMouseButton))
	{
		FRotator rotationChange(mouseY * RotationRate, 0.0f, mouseX * RotationRate);
		AddActorLocalRotation(rotationChange);
	}
}

void APaper::RotateWithDAndA(APlayerController* playerController)
{
	if (playerController == nullptr)
		return;

	bool isDKeyPressed = playerController->IsInputKeyDown(EKeys::D);
	bool isAKeyPressed = playerController->IsInputKeyDown(EKeys::A);

	float rotationChange = 0.0f;

	if (isDKeyPressed)
		rotationChange += RotationRate;
	if (isAKeyPressed)
		rotationChange -= RotationRate;

	FRotator newRotation = GetActorRotation();
	newRotation.Yaw += rotationChange;
	SetActorRotation(newRotation);
}

void APaper::MoveWithMouseWithinRange(APlayerController* playerController, FVector MinRange, FVector MaxRange)
{
	if (playerController == nullptr)
		return;

	float mouseX, mouseY;
	playerController->GetInputMouseDelta(mouseX, mouseY);

	if (playerController->IsInputKeyDown(EKeys::LeftMouseButton))
	{
		FVector CurrentLocation = GetActorLocation();
		FVector LocationChange(mouseX * -1.0f, 0.0f, mouseY);

		FVector NewLocation = CurrentLocation + LocationChange;

		// Clamp the new location within the specified range
		NewLocation.X = FMath::Clamp(NewLocation.X, m_MinMovementPos.X, m_MaxMovementPos.X);
		NewLocation.Y = m_OpenLocation.Y;
		NewLocation.Z = FMath::Clamp(NewLocation.Z, m_MinMovementPos.Y, m_MaxMovementPos.Y);

		SetActorLocation(NewLocation);
	}
}

void APaper::HandleZoomEvent(UCameraComponent* cameraComponent, float value, float zoomSpeed, float MinFOV, float MaxFOV, APlayerController* playerController)
{
	if (cameraComponent == nullptr || playerController == nullptr)
		return;

	float NewFOV = FMath::Clamp(cameraComponent->FieldOfView + (-1 * value) * zoomSpeed, MinFOV, MaxFOV);
	cameraComponent->SetFieldOfView(NewFOV);
}


void APaper::BeginPlay()
{
	Super::BeginPlay();

	m_OrginalLocation = GetActorLocation();
	m_OrginalRotation = GetActorRotation();
}
