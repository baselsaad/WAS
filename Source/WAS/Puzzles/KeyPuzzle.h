// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Puzzles/BasePuzzle.h"
#include "KeyPuzzle.generated.h"

UCLASS()
class WAS_API APiece : public AActor
{
	GENERATED_BODY()

	APiece();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle")
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentIndex;

	UPROPERTY(EditAnywhere)
	int UniqeKey;


	virtual void Tick(float DeltaSeconds) override;

	void DestroyTriggerBox();

	UFUNCTION()
		void OverlapBeginn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
				UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
				bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor
			, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	TSoftObjectPtr<AScientist> m_MainPlayer;
	FRotator m_OrginalRotation;

	friend class AKeyPuzzle;

protected:
	virtual void BeginPlay() override;

};

/**
 * 
 */
UCLASS()
class WAS_API AKeyPuzzle : public ABasePuzzle
{
	GENERATED_BODY()

public:
	AKeyPuzzle();
	
public:
	virtual void Interact(AScientist* mainPlayer) override;
	virtual void Update() override;

	virtual void OnEnterPuzzle() override;
	virtual void OnLeavePuzzle() override;
	virtual bool IsSolved() override;
	void OnGetKeyFromPlayer(APiece* piece, bool load);
	void LeavePuzzle();
	void RotateAnimation();
	

	void HighLightSelectedPieces();
	void SwitchSelectedPieces(int direction);
	void AnimateHighLightedPiece(float DeltaSeconds);

	inline int GetPiecesNum() const { return m_Pieces.Num(); }
	inline const TArray<APiece*>& GetCollectedPieces() const { return m_CollectedPieces; }


	virtual void Tick(float DeltaSeconds) override;


	UPROPERTY(BlueprintReadWrite)
	FRotator AnimRotateDir; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Solved;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimationSpeedRate;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (AllowPrivateAccess = true))
	UMaterial* m_Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KeyPuzzle", meta = (AllowPrivateAccess = true))
	TArray<APiece*> m_Pieces; // Used To Hold The Keys in the world

	// Put the middle piece at middle, at 2 in 7 size array for example 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KeyPuzzle", meta = (AllowPrivateAccess = true , ToolTip = "Put the middle piece at middle, at 2 in 7 size array for example "))
	TArray<FVector> m_PieceHolder; // Key Position, Set this Manually in Editor !!!

	//  Save how many keys the player has collected
	TArray<APiece*> m_CollectedPieces; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (AllowPrivateAccess = true))
	class APostProcessVolume* m_PostProcessingVolume;

	class UMaterialInstanceDynamic* m_DynamicMaterial;

	bool m_ShouldAnimate;
	FTimerHandle m_RotationTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (AllowPrivateAccess = true))
	float AnimRotationRate;

	ACharacter* m_CosutomPivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KeyPuzzle", meta = (AllowPrivateAccess = true))
	TArray<int> m_RandomPositon; // index in place holder
};


