#include "Manager/LevelManager.h"
#include "Tools/MyAssertTool.h"
#include "CarPuzzle.h"


#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Article.h"
#include <EngineUtils.h>
#include <Engine/StaticMesh.h>
#include "ObstaclesManager.h"
#include "OrderPuzzle.h"
#include "Pill.h"
#include "Components/BoxComponent.h"



const FString UGameLoaderAndSaver::SaveSlotName = "WAS_SV";
const FString UGameLoaderAndSaver::SettingsSlotName = "WASSettings_SV";
const uint32 UGameLoaderAndSaver::UserIndex = 0;

UGameLoaderAndSaver::UGameLoaderAndSaver()
{

}

ULevelManager::ULevelManager()
{
}

void ULevelManager::OnStart()
{
	// LoadGame();
}

void ULevelManager::Shutdown()
{
	// SaveGame();
}

void ULevelManager::StartGame()
{
}

void ULevelManager::SaveGame(AScientist* player)
{
	UGameLoaderAndSaver* saveObject = nullptr;

	// Create if not exist or load the data then overwrite it 
	if (UGameplayStatics::DoesSaveGameExist(UGameLoaderAndSaver::SaveSlotName, UGameLoaderAndSaver::UserIndex))
		saveObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::LoadGameFromSlot(UGameLoaderAndSaver::SaveSlotName, UGameLoaderAndSaver::UserIndex));
	else
		saveObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::CreateSaveGameObject(UGameLoaderAndSaver::StaticClass()));

	//Save Information
	{
		if (player)
		{
			saveObject->GameData.Inventory = player->GetInventory();
			saveObject->GameData.CollectedArticles = player->GetInventory().Titles;
			saveObject->GameData.TotalKeysHint = player->GetInventory().totalKeysPickedUp;
			saveObject->GameData.HasMask = player->GetInventory().HasMask;
			saveObject->GameData.Pills = player->GetInventory().Pills;
			SavePlayerCollectedKeys(saveObject, player);
			saveObject->GameData.CurrentPlayerRun = player->GetCurrentRun();
			Debug::LogWarn("Save Player Information");
		}
		SaveOrderPuzzleInfo(saveObject);
		SaveKeyPuzzleInfo(saveObject);
		SaveMoveableObjectsInfo(saveObject);
	}

	if (UGameplayStatics::SaveGameToSlot(saveObject, saveObject->SaveSlotName, saveObject->UserIndex))
		Debug::LogWarn("Saving Data Success");
}

void ULevelManager::SaveKeyPuzzleInfo(UGameLoaderAndSaver* saveObject)
{
	// KeyPuzzle
	AKeyPuzzle* ptr = nullptr;

	// it should be one AKeyPuzzle in the Scene!
	for (TActorIterator<AKeyPuzzle> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ptr = *ActorItr;
	}

	if (ptr == nullptr)
		return;

	for (APiece* piece : ptr->GetCollectedPieces())
	{
		saveObject->GameData.KeyPuzzleCollectedPieces.Add(piece->UniqeKey, piece->CurrentIndex);
	}

	saveObject->GameData.IsKeyPuzzleSolved = ptr->Solved;
}

void ULevelManager::SavePlayerCollectedKeys(UGameLoaderAndSaver* saveObject, AScientist* player)
{
	saveObject->GameData.KeyIndcies.Reset();

	auto& collected = player->GetInventory().CollectedKeys;
	for (APiece* piece : collected)
	{
		saveObject->GameData.KeyIndcies.AddUnique(piece->UniqeKey);
	}
}

void ULevelManager::LoadGame()
{
	if (!UGameplayStatics::DoesSaveGameExist(UGameLoaderAndSaver::SaveSlotName, UGameLoaderAndSaver::UserIndex))
		return;

	UGameLoaderAndSaver* loadObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::LoadGameFromSlot(UGameLoaderAndSaver::SaveSlotName, UGameLoaderAndSaver::UserIndex));
	m_LoadedGame = loadObject->GameData;
	AScientist* player = Cast<AScientist>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (player == nullptr)
		return;

	player->CurrentRun = m_LoadedGame.CurrentPlayerRun;
	player->GetInventory().LightLamp = m_LoadedGame.Inventory.LightLamp;
	player->GetInventory().HasMask = m_LoadedGame.HasMask;
	player->GetInventory().Pills = m_LoadedGame.Pills;
	LoadPills();

	// Get collected Articles
	{
		if (m_LoadedGame.CollectedArticles.Num() > 0)
		{
			for (TActorIterator<AArticle> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				if (m_LoadedGame.CollectedArticles.Contains(ActorItr->Title) && !player->GetInventory().Titles.Contains(ActorItr->Title))
				{
					ActorItr->PickUP(player);

				}
			}
		}
	}

	// Get collected Keys
	{
		if (m_LoadedGame.KeyIndcies.Num() > 0)
		{
			for (TActorIterator<APiece> ActorItr(GetWorld()); ActorItr; ++ActorItr)
				if (m_LoadedGame.KeyIndcies.Contains(ActorItr->UniqeKey))
					player->GetInventory().CollectKey(*ActorItr);
		}
	}

	// Puzzles
	{
		LoadKeyPuzzleInfo();
		player->GetInventory().totalKeysPickedUp = m_LoadedGame.KeyIndcies.Num() + m_LoadedGame.KeyPuzzleCollectedPieces.Num();
		LoadMoveableOjbectsInfo();
		LoadOrderPuzzleInfo();
	}

	ShowAndHideObstacles(player);
}

void ULevelManager::ShowAndHideObstacles(AScientist* player)
{
	if (player == nullptr)
		return;

	AObstaclesManager* ptr = nullptr;
	for (TActorIterator<AObstaclesManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ptr = *ActorItr;
	}

	if (ptr == nullptr)
		return;

	for (auto& pair : ptr->ToShow)
	{
		for (AActor* actor : pair.Value.Obstacles)
		{
			if (actor)
			{
				if (player->GetCurrentRun() == pair.Key)
				{
					actor->SetActorHiddenInGame(false);
					if (!actor->GetActorEnableCollision())
					{
						actor->SetActorEnableCollision(true);
					}
				}
				else
				{
					actor->SetActorHiddenInGame(true);
					if (actor->GetActorEnableCollision())
					{
						actor->SetActorEnableCollision(false);
					}
				}
			}
		}

		
	}
}

void ULevelManager::LoadPills()
{
	APill* ptr = nullptr;
	for (TActorIterator<APill> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ptr = *ActorItr;
		if (ptr)
		{
			for (int id : m_LoadedGame.Pills)
			{
				if (id == ptr->UniqueID)
				{
					// Hide the pill cause its already taken 
					ptr->SetActorHiddenInGame(true);
					if (ptr->CollisionBoxForInteraction)
					{
						ptr->CollisionBoxForInteraction->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					}
				}
			}
		}
	}
}

void ULevelManager::SaveGraphicsSettings(GraphicsOption quality)
{
	UGameLoaderAndSaver* saveObject = nullptr;

	// Create if not exist or load the data then overwrite it 
	if (UGameplayStatics::DoesSaveGameExist(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex))
		saveObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::LoadGameFromSlot(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex));
	else
		saveObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::CreateSaveGameObject(UGameLoaderAndSaver::StaticClass()));

	saveObject->GameData.QualitySettings = quality;

	if (UGameplayStatics::SaveGameToSlot(saveObject, saveObject->SettingsSlotName, saveObject->UserIndex))
		Debug::LogWarn("Saving Data Success");
}

void ULevelManager::SaveShadowSettings(GraphicsOption shadowQuality)
{
	UGameLoaderAndSaver* saveObject = nullptr;

	// Create if not exist or load the data then overwrite it 
	if (UGameplayStatics::DoesSaveGameExist(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex))
		saveObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::LoadGameFromSlot(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex));
	else
		saveObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::CreateSaveGameObject(UGameLoaderAndSaver::StaticClass()));

	saveObject->GameData.ShadowSettings = shadowQuality;

	if (UGameplayStatics::SaveGameToSlot(saveObject, saveObject->SettingsSlotName, saveObject->UserIndex))
		Debug::LogWarn("Saving Data Success");
}

void ULevelManager::SaveRes(const FIntPoint& res)
{
	UGameLoaderAndSaver* saveObject = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex))
		saveObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::LoadGameFromSlot(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex));
	else
		saveObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::CreateSaveGameObject(UGameLoaderAndSaver::StaticClass()));

	saveObject->GameData.ScreenRes = res;

	if (UGameplayStatics::SaveGameToSlot(saveObject, saveObject->SettingsSlotName, saveObject->UserIndex))
		Debug::LogWarn("Saving Data Success");
}

void ULevelManager::ResetGame()
{
	if (HasSavedGame())
	{
		UGameplayStatics::DeleteGameInSlot(UGameLoaderAndSaver::SaveSlotName, UGameLoaderAndSaver::UserIndex);
	}
}

GraphicsOption ULevelManager::LoadGraphicsSettings()
{
	if (!UGameplayStatics::DoesSaveGameExist(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex))
		return GraphicsOption::High;

	UGameLoaderAndSaver* loadObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::LoadGameFromSlot(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex));
	return loadObject->GameData.QualitySettings;
}

GraphicsOption ULevelManager::LoadShadowSettings()
{
	if (!UGameplayStatics::DoesSaveGameExist(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex))
		return GraphicsOption::High;

	UGameLoaderAndSaver* loadObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::LoadGameFromSlot(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex));
	return loadObject->GameData.ShadowSettings;
}

FIntPoint ULevelManager::GetSavedRes()
{
	if (!UGameplayStatics::DoesSaveGameExist(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex))
		return { 1920, 1080 };

	UGameLoaderAndSaver* loadObject = Cast<UGameLoaderAndSaver>(UGameplayStatics::LoadGameFromSlot(UGameLoaderAndSaver::SettingsSlotName, UGameLoaderAndSaver::UserIndex));
	return loadObject->GameData.ScreenRes;
}

bool ULevelManager::HasSavedGame()
{
	return UGameplayStatics::DoesSaveGameExist(UGameLoaderAndSaver::SaveSlotName, UGameLoaderAndSaver::UserIndex);
}

void ULevelManager::LoadKeyPuzzleInfo()
{
	TArray<APiece*> pieces;
	AKeyPuzzle* ptr = nullptr;

	// it should be one AKeyPuzzle in the Scene!
	for (TActorIterator<AKeyPuzzle> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ptr = *ActorItr;
	}

	if (ptr == nullptr)
		return;

	ptr->Solved = m_LoadedGame.IsKeyPuzzleSolved;

	for (TActorIterator<APiece> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (m_LoadedGame.KeyPuzzleCollectedPieces.Contains(ActorItr->UniqeKey))
		{
			ActorItr->CurrentIndex = m_LoadedGame.KeyPuzzleCollectedPieces[ActorItr->UniqeKey];
			pieces.Add(*ActorItr);
		}
	}

	for (auto& piece : pieces)
		ptr->OnGetKeyFromPlayer(piece, true);

}

void ULevelManager::SaveOrderPuzzleInfo(UGameLoaderAndSaver* saveObject)
{
	AOrderPuzzle* ptr = nullptr;
	for (TActorIterator<AOrderPuzzle> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ptr = *ActorItr;
	}

	if (ptr == nullptr)
		return;

	saveObject->GameData.IsOrderPuzzleSolved = ptr->IsSolved();
}

void ULevelManager::LoadOrderPuzzleInfo()
{
	if (!m_LoadedGame.IsOrderPuzzleSolved)
		return;

	AOrderPuzzle* ptr = nullptr;

	// it should be one AKeyPuzzle in the Scene!
	for (TActorIterator<AOrderPuzzle> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ptr = *ActorItr;
	}

	if (ptr == nullptr)
		return;

	ptr->SolvePuzzle();

}

void ULevelManager::SaveMoveableObjectsInfo(UGameLoaderAndSaver* saveObject)
{
	for (TActorIterator<ACarPuzzle> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (saveObject->GameData.MoveableObjectPuzzle.Contains(ActorItr->UniqueKey))
			saveObject->GameData.MoveableObjectPuzzle.Remove(ActorItr->UniqueKey);

		saveObject->GameData.MoveableObjectPuzzle.Add(ActorItr->UniqueKey, ActorItr->GetActorLocation());
	}
}

void ULevelManager::LoadMoveableOjbectsInfo()
{
	for (TActorIterator<ACarPuzzle> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (m_LoadedGame.MoveableObjectPuzzle.Contains(ActorItr->UniqueKey))
		{
			ActorItr->SetActorLocation(m_LoadedGame.MoveableObjectPuzzle[ActorItr->UniqueKey]);
		}
	}
}

