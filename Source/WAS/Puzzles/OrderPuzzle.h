// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Puzzles/BasePuzzle.h"
#include "OrderPuzzle.generated.h"

struct FTimerHandle;
class AArticle;

#define NOT_NOTE -1

UENUM(BlueprintType)
enum class PaperType : uint8
{
	PAPER, NOTE
};

UCLASS()
class WAS_API APaper : public AActor
{
	GENERATED_BODY()

public:
	APaper();

	void ClosePaper();
	void RotateWithMouse(APlayerController* playerController);
	void RotateWithDAndA(APlayerController* playerController);
	void MoveWithMouseWithinRange(APlayerController* playerController, FVector MinRange, FVector MaxRange);
	void HandleZoomEvent(UCameraComponent* cameraComponent, float value, float zoomSpeed, float MinFOV, float MaxFOV, APlayerController* playerController);
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	PaperType PaperTyp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // Used to check (if this is a node) to which paper 
		int CorrectIndex;

	inline const FVector& GetOrginalLocation() { return m_OrginalLocation; }
	inline const FRotator& GetOrginalRotation() { return m_OrginalRotation; }

	inline void SetNewOrginalPosition(FVector& location) { m_OrginalLocation = location; }
	inline void SetNewOrginalRotation(FRotator& rotation) { m_OrginalRotation = rotation; }

private:
	FVector m_OrginalLocation;
	FVector m_OpenLocation;
	FRotator m_OrginalRotation;

	FVector2D m_MaxMovementPos;
	FVector2D m_MinMovementPos;

	friend class AOrderPuzzle;

protected:
	virtual void BeginPlay() override;
};


/**
 *
 */
UCLASS()
class WAS_API AOrderPuzzle : public ABasePuzzle
{
	GENERATED_BODY()

public:
	AOrderPuzzle();

public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void Interact(AScientist* mainPlayer) override;
	virtual void Update() override;
	virtual bool IsSolved() override;
	virtual void OnEnterPuzzle() override;
	virtual void OnLeavePuzzle() override;

	void SolvePuzzle();
	void SwapNoteWithA();
	void SwapNoteWithD();
	void HandleZoomEvent(float value);

private:
	void LeavePuzzle();
	void OnRightMouseClicked();
	void OnLeftMouseClicked();
	void OpenPaper();


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<APaper*> m_Papers; // Map of Papers with correct solution

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<APaper*> m_Notes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<AArticle*> m_Articles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float m_DistanceFromCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float m_LerpSpeed;

	APaper* m_CurrentOpendPaper;

	FTimerHandle PaperAnimationTimer;


protected:
	virtual void BeginPlay() override;


	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	UPROPERTY(EditAnywhere, Category = "sound")
	class USoundBase* PostItRemove;

	UPROPERTY(EditAnywhere, Category = "sound")
	class USoundBase* PostItRemove2;

	UPROPERTY(EditAnywhere, Category = "sound")
	class USoundBase* Order1;

	UPROPERTY(EditAnywhere, Category = "sound")
	class USoundBase* Order2;

	UPROPERTY(EditAnywhere, Category = "sound")
	class USoundBase* Paper;

	UPROPERTY(EditAnywhere)
	float ZoomSpeed;

	UPROPERTY(EditAnywhere)
	float MinFOV;

	UPROPERTY(EditAnywhere)
	float MaxFOV;


private:
	bool m_WasSolved;
};
