#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarPuzzle.generated.h"

class UArrowComponent;

UCLASS()
class WAS_API ACarPuzzle : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACarPuzzle();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
		void OverlapBeginn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor
			, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isMovingForword;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsMovingSideway;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int UniqueKey; // used for save and load

private:
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* BoxMesh;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* m_Trigger;

	UPROPERTY(EditAnywhere)
	class UArrowComponent* m_Arrow; //Used to choose direction of the object

	void OnKeyPressed();
	void OnKeyReleased();

	bool CheckCollision(const FVector& Direction);
	bool m_IsInsideBox;
	bool m_IsAttached;
	bool m_KeyIsPressed;
	class AScientist* m_Player;
};