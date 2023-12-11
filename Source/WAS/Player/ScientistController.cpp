// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ScientistController.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/GameUserSettings.h>
#include "LevelManager.h"
#include <Kismet/GameplayStatics.h>
#include "Debug.h"

void AScientistController::EnableDepthOfFieldOnObject(UCameraComponent* camera, float distance)
{
	if (camera == nullptr)
		return;

	// Enable depth of field
	camera->PostProcessSettings.bOverride_DepthOfFieldDepthBlurAmount = true;
	camera->PostProcessSettings.bOverride_DepthOfFieldScale = true;
	camera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	camera->PostProcessSettings.DepthOfFieldFocalDistance = distance;

	float Aperture = 1.0f;
	camera->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
	camera->PostProcessSettings.DepthOfFieldFstop = Aperture;
}

void AScientistController::DisableDepthOfField(UCameraComponent* camera)
{
	if (camera == nullptr)
		return;

	// Disable depth of field by overriding the relevant parameters
	camera->PostProcessSettings.bOverride_DepthOfFieldDepthBlurAmount = false;
	camera->PostProcessSettings.bOverride_DepthOfFieldScale = false;
	camera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = false;
	camera->PostProcessSettings.bOverride_DepthOfFieldFstop = false;
}

void AScientistController::SetQuality(GraphicsOption op)
{
	int quality = (int)op;

	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	UserSettings->SetFoliageQuality(quality);
	UserSettings->SetTextureQuality(quality);
	UserSettings->SetShadingQuality(quality);
	UserSettings->SetAntiAliasingQuality(quality);
	UserSettings->SetViewDistanceQuality(quality);
	UserSettings->SetPostProcessingQuality(quality);
	UserSettings->SetVisualEffectQuality(quality);

	UserSettings->ApplySettings(true);
}

void AScientistController::SetShadowQuality(GraphicsOption op)
{
	int quality = (int)op;

	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	UserSettings->SetShadowQuality(quality);
	UserSettings->ApplySettings(true);
}

void AScientistController::SetScreenRes(const FIntPoint& res)
{
	UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
	UserSettings->SetScreenResolution(res);
	UserSettings->ApplyResolutionSettings(true);
	UserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
	UserSettings->ApplySettings(true);
}

void AScientistController::NeuesSpiel()
{
	UE_LOG(LogTemp, Warning, TEXT("Neues Spiel"));
}

void AScientistController::BeginPlay()
{
	Super::BeginPlay();

	auto levelManager = Cast<ULevelManager>(UGameplayStatics::GetGameInstance(this));
	if (levelManager)
	{
		int quality = (int)levelManager->LoadGraphicsSettings();
		UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
		UserSettings->SetFoliageQuality(quality);
		UserSettings->SetTextureQuality(quality);
		UserSettings->SetShadingQuality(quality);
		UserSettings->SetAntiAliasingQuality(quality);
		UserSettings->SetViewDistanceQuality(quality);
		UserSettings->SetPostProcessingQuality(quality);
		UserSettings->SetShadowQuality((int)levelManager->LoadShadowSettings());
		UserSettings->SetVisualEffectQuality(quality);
		UserSettings->SetFrameRateLimit(120);
		UserSettings->SetScreenResolution(levelManager->GetSavedRes());
		UserSettings->ApplySettings(false);
		UserSettings->ApplyResolutionSettings(false);

		Debug::LogWarn("res: %s", *UserSettings->GetScreenResolution().ToString());
		Debug::LogWarn("res: ", UserSettings->IsScreenResolutionDirty());
	}

	////Getting a reference to the Game User Settings.
	
}
