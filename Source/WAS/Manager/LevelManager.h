#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/SaveGame.h"
#include "Scientist.h"
#include "Player/ScientistController.h"
#include "LevelManager.generated.h"

class UGameWidget;
class ULevel;
class UWorld;


USTRUCT(BlueprintType)
struct FData
{
	GENERATED_BODY()

public:
	FData()
	{
	}
	
	UPROPERTY(EditAnywhere, Category = Basic)
	FInventory Inventory;

	UPROPERTY(EditAnywhere, Category = Basic)
	TArray<int> KeyIndcies;

	UPROPERTY(EditAnywhere, Category = Basic)
	TArray<FString> CollectedArticles; // Titles because they are unique

	UPROPERTY(EditAnywhere, Category = Basic)
	TMap<int, int> KeyPuzzleCollectedPieces; 

	UPROPERTY(EditAnywhere, Category = Basic)
	bool IsKeyPuzzleSolved;

	UPROPERTY(EditAnywhere, Category = Basic)
	bool HasMask;

	UPROPERTY(EditAnywhere, Category = Basic)
	TMap<int, FVector> MoveableObjectPuzzle;

	UPROPERTY(EditAnywhere, Category = Basic)
	int CurrentPlayerRun;

	UPROPERTY(EditAnywhere, Category = Basic)
	GraphicsOption QualitySettings;

	UPROPERTY(EditAnywhere, Category = Basic)
	GraphicsOption ShadowSettings;

	UPROPERTY(EditAnywhere, Category = Basic)
	FIntPoint ScreenRes;

	UPROPERTY(EditAnywhere, Category = Basic)
	int TotalKeysHint;

	UPROPERTY(EditAnywhere, Category = Basic)
	bool IsOrderPuzzleSolved;

	UPROPERTY(EditAnywhere, Category = Basic)
	TArray<int> Pills;
};


UCLASS(NotBlueprintable, notplaceable)
class UGameLoaderAndSaver : public USaveGame
{
	GENERATED_BODY()

public:
	UGameLoaderAndSaver();

	static const FString SaveSlotName;
	static const FString SettingsSlotName;
	static const uint32 UserIndex;
	
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FData GameData;
};

UCLASS()
class WAS_API ULevelManager : public UGameInstance
{
	GENERATED_BODY()

public:
	ULevelManager();

public:
	void StartGame();
	
	//this should be called only from GameManager
	void SaveGame(AScientist* player);

	// Player Data
	void SavePlayerCollectedKeys(UGameLoaderAndSaver* saveObject, AScientist* player);
	
	//Puzzles Data
	void SaveKeyPuzzleInfo(UGameLoaderAndSaver* saveObject);
	void LoadKeyPuzzleInfo();

	void SaveOrderPuzzleInfo(UGameLoaderAndSaver* saveObject);
	void LoadOrderPuzzleInfo();

	void SaveMoveableObjectsInfo(UGameLoaderAndSaver* saveObject);
	void LoadMoveableOjbectsInfo();

	void LoadGame();
	bool HasSavedGame();
	void ShowAndHideObstacles(AScientist* player);
	void LoadPills();

	void SaveGraphicsSettings(GraphicsOption quality);
	void SaveShadowSettings(GraphicsOption shadowQuality);
	void SaveRes(const FIntPoint& res);

	void ResetGame();

	GraphicsOption LoadGraphicsSettings();
	GraphicsOption LoadShadowSettings();
	FIntPoint GetSavedRes();

protected:
	virtual void OnStart() override;
	virtual void Shutdown() override;

private:
	UPROPERTY(EditAnywhere, meta = (DisplayName = "DoNotLoadFromDisk", AllowPrivateAccess = true))
	bool m_DoNotLoadFromDisk;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "LoadedGame", AllowPrivateAccess = true))
	FData m_LoadedGame;
};



