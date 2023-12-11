#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "MasK.generated.h"

UCLASS()
class WAS_API AMask : public ABaseItem
{
	GENERATED_BODY()



public:
	AMask();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mask;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBoxForInteraction;


	bool IsOnTheCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OxygenMeter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxOxygenMeter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinOxygenMeter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DecressOxygenMeter;

	inline bool IsEmpty() const { return m_IsEmpty; }
	
	UFUNCTION()
	void OxygenRefill();

	UFUNCTION()
	void OxygenDepletion();




	virtual void  PickUP(AScientist* mainPlayer) override;
private:
	void WearMask(AScientist* mainPlayer);

	bool m_IsEmpty;


};
