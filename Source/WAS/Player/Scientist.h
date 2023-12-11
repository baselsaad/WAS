// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/Article.h"
#include "Puzzles/KeyPuzzle.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Scientist.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UCameraShakeBase;
class AScientistController;
class ABaseItem;
class ABasePuzzle;
class AMask;
class AOxygenTank;
class UScreenAnim;
class UMediaSoundComponent;
class UMediaPlayer;

USTRUCT(BlueprintType)
struct FInventory
{
	GENERATED_BODY()

	TArray<APiece*> CollectedKeys; // Here the collected keys will be saved until the player set it in "Puzzle Place"
	TArray<TSoftObjectPtr<AArticle>> CollectedArticles;

	UPROPERTY(EditAnywhere)
	TArray<FString> Titles;

	UPROPERTY(EditAnywhere)
	TArray<FString> Contents;

	UPROPERTY(EditAnywhere)
	TArray<int> Pills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LightLamp = false;

	bool HasMask = false;
	int totalKeysPickedUp = 0; //used for hint messagel,because inventory keys can decrease when placed in puzzle

	bool CollectKey(APiece* piece);

	inline bool CollectArticle(AArticle* article)
	{
		if (article == nullptr)
			return false;

		CollectedArticles.Add(article);
		Titles.Add(article->Title);
		Contents.Add(article->Content);

		return true;
	}

	inline TArray<TSoftObjectPtr<AArticle>>& GetAllArticles()
	{
		return CollectedArticles;
	}

	bool GiveCollectedKeyPiceeUp(AKeyPuzzle& puzzle);
};


UCLASS()
class WAS_API AScientist : public ACharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AScientist();
	~AScientist();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	void SetupFirstPersonInputs(UInputComponent* InputComponent);

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	/** Handles moving forward/backward */
	void MoveForward(float val);

	/** Handles staging movement, left and right */
	void MoveRight(float val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyActorEndOverlap(AActor* OtherActor) override;

	void OpenInventory();

	UFUNCTION(BlueprintCallable)
	void SetUIInputOnly();

	UFUNCTION(BlueprintCallable)
	void SetGameInputOnly();

	/*
	* API
	*/
	void PlayCameraShakeEffect(TSubclassOf<class UCameraShakeBase> shaker, float scale);
	void EquipItem(ABaseItem* item);
	void CreateInventory();
	void DestroyInventory();

	void OnDeath();
	void OnStartRunning();
	void OnStopRunning();
	void OnStartMemory();
	void OnStopMemory();
	void ComputeRayCastInformation(FVector& outStart, FVector& outEnd, float maxRange);
	void PlayFirstMemVideo();
	void PlaySecMemVideo();
	void OnPlayingVideoEnds();
	void SkipVideo();
	void EndGame();

	// Puzzle-API
	void SwitchPlayerCamera(UCameraComponent* camera);
	void ClearAllPlayerInputs();
	void RestartAllPlayerInputs();
	void Interact();
	void OnLeavePuzzle();
	void OnEnterCave();

	void PickUP();

	//inline USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	//inline UAnimInstance* GetAnimationInstance() const { return Mesh1P->GetAnimInstance(); }

	inline UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	inline bool IsDead() { return m_IsDead; }
	inline AScientistController* GetPlayerController() const { return m_PlayerController; }

	inline const TArray<APiece*>& GetPlayerCollectedPieces() { return m_Inventory.CollectedKeys; }

	UFUNCTION(BlueprintCallable)
	inline FInventory& GetInventory() { return m_Inventory; }

	UFUNCTION(BlueprintCallable)
	inline bool HasLightLamp() { return m_Inventory.LightLamp; }

	inline AMask* GetMask() { return m_CurrentMask; }
	inline int GetCurrentRun() const { return CurrentRun; }
	inline void IncreamntCurrnetRun() { CurrentRun++; }

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	///** Pawn mesh: 1st person view (arms; seen only by self) */
	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	//USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	float CameraSwitchBlendTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	int CurrentRun; //Save in which run the player is now 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	TSubclassOf<UScreenAnim> SceneAnimWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	TSubclassOf<UScreenAnim> GameOverAnimWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	TSubclassOf<UScreenAnim> MemVideoWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	TSubclassOf<UScreenAnim> CreaditSceneWidget;

	UPROPERTY(EditAnywhere, Category = "sound")
	class USoundBase* SoundMemVideo1;

	UPROPERTY(EditAnywhere, Category = "sound")
	class USoundBase* SoundMemVideo2;

	UPROPERTY(EditAnywhere, Category = "sound")
	class USoundBase* EndSound;

private:
	FCollisionQueryParams m_HitParams;//used for ray casting, instead of init on every ray shoot!
	AScientistController* m_PlayerController;
	ABasePuzzle* m_CurrentPuzzle;
	FTimerHandle GroundDetectorTimer;
	FTimerHandle m_VideoMemoryTimer;

	FInventory m_Inventory;
	ABaseItem* m_CurrentItem;
	AMask* m_CurrentMask;

	UScreenAnim* m_ScreenAnimWidget;
	UScreenAnim* m_GameOverAnimWidget;
	UScreenAnim* m_MemVideo;
	UScreenAnim* m_CreditScene;
	UAudioComponent* m_MemVideoAudio;


	bool m_IsDead;
	bool m_GameEnd;
	bool m_BeginToDie;
	bool m_IsInsidePuzzleBox; // used for trigger box 

	float m_DieTimer;

	//ugly voiceline implementaion
	int num;
	int frequency;
};
