#include "Scientist.h"

#include "Player/ScientistController.h"
#include "Tools/Debug.h"
#include "Items/BaseItem.h"
#include "Tools/MyAssertTool.h"
#include "Puzzles/BasePuzzle.h"
#include "Menu/GameHudWidget.h"
#include "Menu/InventoryWidget.h"
#include "Items/flashlight.h"
#include "Voicelines.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ProgressBar.h"
#include "MasK.h"
#include "BaseHud.h"
#include "LevelManager.h"
#include "Kismet/GameplayStatics.h"
#include <DrawDebugHelpers.h>
#include "ScreenAnim.h"
#include <EngineUtils.h>
#include <MediaSoundComponent.h>
#include <MediaPlayer.h>



static bool s_MaskHintShown = false;

// Sets default values
AScientist::AScientist()
	: BaseTurnRate(1.0f)
	, BaseLookUpRate(1.0f)
	, WalkSpeed(100.0f)
	, RunSpeed(500.0f)
	, m_IsInsidePuzzleBox(false)
	, m_CurrentPuzzle(nullptr)
	, m_CurrentMask(nullptr)
	, CameraSwitchBlendTime(0.1f)
	, CurrentRun(1)
	, m_BeginToDie(false)
	, m_DieTimer(0.0f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetMesh()->SetupAttachment(FirstPersonCameraComponent);
	GetMesh()->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	GetMesh()->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	GetMesh()->bCastDynamicShadow = false;
	GetMesh()->CastShadow = false;

	m_MemVideoAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioForMemoryVideo"));
	m_MemVideoAudio->SetupAttachment(RootComponent);
	m_MemVideoAudio->SetVolumeMultiplier(200.0f);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore); // Ignore mouse 

	num = 0; //for voicelines (will be changed)
	frequency = 100;
}

AScientist::~AScientist()
{
}

// Called when the game starts or when spawned
void AScientist::BeginPlay()
{
	Super::BeginPlay();

	if (m_GameOverAnimWidget)
	{
		m_GameOverAnimWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	m_PlayerController = Cast<AScientistController>(GetController());
	ASSERT(m_PlayerController, "Set The Player Controller in the World Settings!");

	GetWorld()->GetGameInstanceChecked<ULevelManager>()->LoadGame();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if (m_Inventory.HasMask)
	{
		AMask* ptr = nullptr;
		for (TActorIterator<AMask> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ptr = *ActorItr;

			if (ptr)
			{
				m_CurrentMask = ptr;
				m_CurrentMask->PickUP(this);
			}
		}
	}

	if (UGameHudWidget::Get())
	{
		UGameHudWidget::Get()->ClearDialogText();


		UGameHudWidget::Get()->AddDialogText(FText::FromString("WASD"), FText::FromString("Bewegung"));
		UGameHudWidget::Get()->AddDialogText(FText::FromString("Shift"), FText::FromString("Sprinten"));
		UGameHudWidget::Get()->AddDialogText(FText::FromString("I"), FText::FromString("Inventory"));


		FTimerHandle timer;
		GetWorldTimerManager().SetTimer(timer, [this]() {UGameHudWidget::Get()->ClearDialogText(); }, 3.0f, false);
	}
}

void AScientist::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetGameInstanceChecked<ULevelManager>()->SaveGame(this);
}

// Called to bind functionality to input
void AScientist::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	SetupFirstPersonInputs(PlayerInputComponent);
}

void AScientist::SetupFirstPersonInputs(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// Running
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AScientist::OnStartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AScientist::OnStopRunning);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AScientist::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AScientist::MoveRight);

	//Windows
	PlayerInputComponent->BindAxis("Turn", this, &AScientist::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AScientist::LookUpAtRate);

	// Interact with puzzles, doors etc..
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AScientist::Interact);
	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AScientist::PickUP);


	PlayerInputComponent->BindAction("OpenInventory", IE_Pressed, this, &AScientist::OpenInventory);
}

// Called every frame
void AScientist::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_GameEnd)
		return;

	FVector start = GetActorLocation();
	FVector end = start - FVector(0.0f, 0.0f, 200.0f); // Raycast down 200 units

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // Ignore the character itself

	if (GetWorld()->LineTraceSingleByChannel(HitResult, start, end, ECC_Visibility, Params))
	{
		FString HitActorName = HitResult.Actor->GetName();
		if (HitActorName == "untitled_map_undergroundsystem_cave_3")
		{
			if (!GetWorldTimerManager().IsTimerActive(GroundDetectorTimer))
			{
				GetWorldTimerManager().SetTimer(GroundDetectorTimer, this, &AScientist::OnEnterCave, 5.0f, true);
				m_DieTimer = 5.0f;
			}

			if ((!m_CurrentMask || m_CurrentMask->IsEmpty()) && m_BeginToDie)
			{
				m_DieTimer -= DeltaTime;
				m_DieTimer = FMath::Clamp(m_DieTimer, 0.0f, 5.0f);
				UGameHudWidget::Get()->ClearHintText();
				FString hint = FString::Printf(TEXT("Verbleibende Zeit: %.2f Sekunden. Denk daran, eine gefuellte Maske zu tragen!"), m_DieTimer);
				UGameHudWidget::Get()->AddHintText(FText::FromString(hint));
				s_MaskHintShown = true;
			}
		}
		else
		{
			GetWorldTimerManager().ClearTimer(GroundDetectorTimer);
			if (s_MaskHintShown)
			{
				UGameHudWidget::Get()->ClearHintText();
				s_MaskHintShown = false;
			}
			m_BeginToDie = false;

			if (!m_IsDead)
			{
				if (m_ScreenAnimWidget)
				{
					m_ScreenAnimWidget->SetVisibility(ESlateVisibility::Hidden);
				}

				if (m_DieTimer > 0.0f)
					m_DieTimer = 0.0f;
			}
		}
	}
	else
	{
		m_BeginToDie = false;
		GetWorldTimerManager().ClearTimer(GroundDetectorTimer);
		if (s_MaskHintShown)
		{
			UGameHudWidget::Get()->ClearHintText();
			s_MaskHintShown = false;
		}

		if (!m_IsDead)
		{
			if (m_ScreenAnimWidget)
			{
				m_ScreenAnimWidget->SetVisibility(ESlateVisibility::Hidden);
			}

			if (m_DieTimer > 0.0f)
				m_DieTimer = 0.0f;
		}
	}
}

void AScientist::OnEnterCave()
{
	if (!m_CurrentMask || m_CurrentMask->IsEmpty())
	{
		if (!m_BeginToDie) // Chance for player to recover
		{
			if (!m_ScreenAnimWidget)
			{
				m_ScreenAnimWidget = CreateWidget<UScreenAnim>(GetWorld(), SceneAnimWidget);
				m_ScreenAnimWidget->AddToViewport();
				m_ScreenAnimWidget->SetOwningPlayer(GetPlayerController());
			}

			m_ScreenAnimWidget->SetVisibility(ESlateVisibility::Visible);
			m_ScreenAnimWidget->DeathAnim();

			m_BeginToDie = true;
		}
		else
		{
			OnDeath();
		}
	}
	else
	{
		m_CurrentMask->OxygenDepletion();

		if (m_BeginToDie)
			m_BeginToDie = false;

		if (m_ScreenAnimWidget)
		{
			m_ScreenAnimWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		if (s_MaskHintShown)
		{
			UGameHudWidget::Get()->ClearHintText();
			s_MaskHintShown = false;
		}

		if (GetWorldTimerManager().IsTimerActive(GroundDetectorTimer))
			GetWorldTimerManager().ClearTimer(GroundDetectorTimer);
	}
}

void AScientist::NotifyActorBeginOverlap(AActor* OtherActor)
{
	m_CurrentPuzzle = Cast<ABasePuzzle>(OtherActor);
	if (m_CurrentPuzzle != nullptr)
	{
		m_IsInsidePuzzleBox = true;
		if (!m_CurrentPuzzle->IsSolved())
		{
			UGameHudWidget::Get()->AddDialogText(FText::FromString("E"), FText::FromString("Interagieren"));
		}
		else
		{
			m_CurrentPuzzle = nullptr;
		}
	}

	m_CurrentItem = Cast<ABaseItem>(OtherActor);
	if (m_CurrentItem)
	{
		UGameHudWidget::Get()->AddDialogText(FText::FromString("E"), FText::FromString("Nehmen"));
	}
}

void AScientist::NotifyActorEndOverlap(AActor* OtherActor)
{
	m_CurrentPuzzle = Cast<ABasePuzzle>(OtherActor);
	if (m_CurrentPuzzle != nullptr)
	{
		m_IsInsidePuzzleBox = false;
		m_CurrentPuzzle = nullptr;
		if (UGameHudWidget::Get())
			UGameHudWidget::Get()->ClearDialogText();
	}

	m_CurrentItem = Cast<ABaseItem>(OtherActor);
	if (m_CurrentItem)
	{
		if (UGameHudWidget::Get())
			UGameHudWidget::Get()->ClearDialogText();
		m_CurrentItem = nullptr;
	}
}

void AScientist::OpenInventory()
{
	auto hud = Cast<ABaseHud>(m_PlayerController->GetHUD());
	if (hud)
	{
		auto inventoryWidget = hud->GetInventoryWidget();
		if (inventoryWidget == nullptr || hud->GetInventoryWidget()->Visibility == ESlateVisibility::Hidden)
		{
			ClearAllPlayerInputs();
			InputComponent->BindAction("OpenInventory", IE_Pressed, this, &AScientist::OpenInventory);
			hud->ShowInventoryWidget();
			SetUIInputOnly();
		}
		else
		{
			ClearAllPlayerInputs();
			RestartAllPlayerInputs();
			SetGameInputOnly();
			hud->HideInventoryWidget();
		}
	}
}

void AScientist::SetUIInputOnly()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	GetPlayerController()->SetInputMode(InputMode);
	GetPlayerController()->bShowMouseCursor = true;
}

void AScientist::SetGameInputOnly()
{
	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	GetPlayerController()->SetInputMode(InputMode);
	GetPlayerController()->bShowMouseCursor = false;
}

void AScientist::MoveForward(float val)
{
	AddMovementInput(GetActorForwardVector(), val);

	if (num % frequency == 0)
	{
		// random walking voicelines
		frequency = frequency + 100;
		num = 0;
		enum EEventType puzzleEvent = EEventType::WALKING;
		enum EEventProgress puzzleEventProgress = EEventProgress::ONGOING;
		AVoicelines::Get()->PlayVoiceLinesIfPossible(puzzleEvent, puzzleEventProgress, Priority::Low);
	}
	num++;

}

void AScientist::MoveRight(float val)
{
	AddMovementInput(GetActorRightVector(), val);
}

void AScientist::TurnAtRate(float Rate)
{
	ASSERT_BREAK(m_PlayerController, "Player Controlelr is null, do not forget to set it in the editor!");

	m_PlayerController->AddYawInput(Rate * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
}

void AScientist::LookUpAtRate(float Rate)
{
	ASSERT_BREAK(m_PlayerController, "Player Controlelr is null, do not forget to set it in the editor!");

	m_PlayerController->AddPitchInput(Rate * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
}

void AScientist::PlayCameraShakeEffect(TSubclassOf<class UCameraShakeBase> shaker, float scale)
{
}

void AScientist::EquipItem(ABaseItem* item)
{

}

void AScientist::CreateInventory()
{

}

void AScientist::DestroyInventory()
{
}

void AScientist::OnDeath()
{
	//Debug::LogWarn("OnDeath");
	m_IsDead = true;
	GetWorldTimerManager().ClearTimer(GroundDetectorTimer);

	if (!m_GameOverAnimWidget)
	{
		m_GameOverAnimWidget = CreateWidget<UScreenAnim>(GetWorld(), GameOverAnimWidget);
		m_GameOverAnimWidget->AddToViewport();
		m_GameOverAnimWidget->SetOwningPlayer(GetPlayerController());
	}

	m_GameOverAnimWidget->SetVisibility(ESlateVisibility::Visible);
	m_GameOverAnimWidget->GameOverAnim();
}

void AScientist::OnStartRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AScientist::OnStopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AScientist::OnStartMemory()
{
	GetCharacterMovement()->MaxWalkSpeed = 0;
}

void AScientist::OnStopMemory()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AScientist::ComputeRayCastInformation(FVector& outStart, FVector& outEnd, float maxRange)
{
	ASSERT_BREAK(m_PlayerController, "Player Controlelr is null, do not forget to set it in the editor!");

	FVector camLocation;
	FRotator camRotation;
	m_PlayerController->GetPlayerViewPoint(camLocation, camRotation);
	const FVector aimDir = camRotation.Vector();

	// StartVector
	{
		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		outStart = camLocation + aimDir * ((GetInstigator()->GetActorLocation() - camLocation) | aimDir);
	}

	// EndVector
	{
		outEnd = outStart + (aimDir * maxRange);
	}
}

void AScientist::PlayFirstMemVideo()
{
	if (!m_MemVideo)
	{
		m_MemVideo = CreateWidget<UScreenAnim>(GetWorld(), MemVideoWidget);
		m_MemVideo->AddToViewport();
		m_MemVideo->SetOwningPlayer(GetPlayerController());
	}

	AVoicelines::Get()->StopAll();
	m_MemVideo->SetVisibility(ESlateVisibility::Visible);
	m_MemVideo->PlayVideo1();
	m_MemVideoAudio->SetSound(SoundMemVideo1);
	m_MemVideoAudio->Play();
	ClearAllPlayerInputs();
	InputComponent->BindAction("SkipVideo", IE_Pressed, this, &AScientist::SkipVideo);

	GetWorldTimerManager().SetTimer(m_VideoMemoryTimer, [this]() { OnPlayingVideoEnds(); }, SoundMemVideo1->GetDuration(), false);
}

void AScientist::PlaySecMemVideo()
{
	if (!m_MemVideo)
	{
		m_MemVideo = CreateWidget<UScreenAnim>(GetWorld(), MemVideoWidget);
		m_MemVideo->AddToViewport();
		m_MemVideo->SetOwningPlayer(GetPlayerController());
	}

	AVoicelines::Get()->StopAll();
	m_MemVideo->SetVisibility(ESlateVisibility::Visible);
	m_MemVideo->PlayVideo2();
	m_MemVideoAudio->SetSound(SoundMemVideo2);
	m_MemVideoAudio->Play();
	ClearAllPlayerInputs();
	InputComponent->BindAction("SkipVideo", IE_Pressed, this, &AScientist::SkipVideo);

	GetWorldTimerManager().SetTimer(m_VideoMemoryTimer, [this]() { OnPlayingVideoEnds(); }, SoundMemVideo2->GetDuration(), false);
}

void AScientist::OnPlayingVideoEnds()
{
	Debug::LogWarn("Video Ends");
	RestartAllPlayerInputs();
	if (m_MemVideo)
	{
		m_MemVideo->SetVisibility(ESlateVisibility::Hidden);
	}

	int pills = m_Inventory.Pills.Num();
	if (pills == 1)
	{
		AVoicelines::Get()->PlayVoiceLinePill(ANSI_TO_TCHAR("/Game/Sound/pill_start_1_Cue"));
	}
	else if (pills == 3)
	{
		AVoicelines::Get()->PlayVoiceLinePill(ANSI_TO_TCHAR("/Game/Sound/pill_start_2_Cue"));
	}
}

void AScientist::SkipVideo()
{
	if (m_MemVideo)
	{
		m_MemVideo->SkipVideo();
	}

	OnPlayingVideoEnds();
	if (m_MemVideoAudio)
	{
		m_MemVideoAudio->Stop();
	}

	if (GetWorldTimerManager().IsTimerActive(m_VideoMemoryTimer))
	{
		GetWorldTimerManager().ClearTimer(m_VideoMemoryTimer);
	}
}

void AScientist::EndGame()
{
	GetWorldTimerManager().ClearTimer(GroundDetectorTimer);
	m_GameEnd = true;
	ClearAllPlayerInputs();
	AVoicelines::Get()->StopAll();

	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, [this]() 
	{ 
		m_MemVideoAudio->SetSound(EndSound);
		m_MemVideoAudio->Play();

		FTimerHandle t;
		GetWorldTimerManager().SetTimer(t, [this]() { UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu")); }, EndSound->GetDuration(), false);

	}, 6.0f, false);

	if (!m_CreditScene)
	{
		m_CreditScene = CreateWidget<UScreenAnim>(GetWorld(), CreaditSceneWidget);
		m_CreditScene->AddToViewport();
		m_CreditScene->SetOwningPlayer(GetPlayerController());
	}

	m_CreditScene->SetVisibility(ESlateVisibility::Visible);
	m_CreditScene->GameOverAnim();
}

void AScientist::SwitchPlayerCamera(UCameraComponent* camera)
{
	if (camera == nullptr)
		m_PlayerController->SetViewTargetWithBlend(this, CameraSwitchBlendTime);
	else
		m_PlayerController->SetViewTargetWithBlend(m_CurrentPuzzle, CameraSwitchBlendTime);
}

void AScientist::ClearAllPlayerInputs()
{
	InputComponent->ClearActionBindings();
	InputComponent->AxisBindings.Empty();
}

void AScientist::RestartAllPlayerInputs()
{
	SetupFirstPersonInputs(InputComponent);
}

bool FInventory::CollectKey(APiece* piece)
{
	if (piece == nullptr)
		return false;

	bool shouldAdd = true;
	for (auto& v : CollectedKeys)
	{
		if (piece->UniqeKey == v->UniqeKey)
			shouldAdd = false;
	}

	if (shouldAdd)
	{
		CollectedKeys.Add(piece);
		totalKeysPickedUp++;
		piece->SetActorHiddenInGame(true);
		piece->DestroyTriggerBox();
	}

	if (UGameHudWidget::Get())
	{
		UGameHudWidget::Get()->ClearDialogText();
		UGameHudWidget::Get()->AddDialogText(FText::FromString(""), FText::Format(NSLOCTEXT("KeysFound", "KeysFound", "{0} von 7 gefunden"), FText::AsNumber(totalKeysPickedUp)), 2);
	}

	AScientistController* playerController = Cast<AScientistController>(UGameplayStatics::GetPlayerController(GEngine->GameViewport->GetWorld(), 0));
	auto hud = Cast<ABaseHud>(playerController->GetHUD());
	hud->GetInventoryWidget()->UpdateItems();

	return shouldAdd;
}

void AScientist::Interact()//Called when player press "E"
{
	if (!m_IsInsidePuzzleBox || m_CurrentPuzzle == nullptr)
		return;

	Debug::LogWarn("Interact");

	UGameHudWidget::Get()->ClearDialogText();
	m_CurrentPuzzle->Interact(this);
}

void AScientist::OnLeavePuzzle()
{
	if (m_CurrentPuzzle && m_CurrentPuzzle->IsSolved())
	{
		m_CurrentPuzzle = nullptr;
		UGameHudWidget::Get()->ClearDialogText();
	}
}



bool FInventory::GiveCollectedKeyPiceeUp(AKeyPuzzle& puzzle)
{
	if (CollectedKeys.Num() == 0)
		return false;

	for (int i = 0; i < CollectedKeys.Num(); i++)
	{
		puzzle.OnGetKeyFromPlayer(CollectedKeys[i], false);
	}

	CollectedKeys.Reset();

	AScientistController* playerController = Cast<AScientistController>(UGameplayStatics::GetPlayerController(GEngine->GameViewport->GetWorld(), 0));
	auto hud = Cast<ABaseHud>(playerController->GetHUD());
	hud->GetInventoryWidget()->UpdateItems();

	return true;
}

void AScientist::PickUP()
{
	if (m_CurrentItem == nullptr)
		return;

	if (m_CurrentMask == nullptr)
	{
		m_CurrentMask = Cast<AMask>(m_CurrentItem);
		m_Inventory.HasMask = true;
	}

	UGameHudWidget::Get()->ClearDialogText();
	m_CurrentItem->PickUP(this);

	if (Cast<Aflashlight>(m_CurrentItem))
	{
		m_Inventory.LightLamp = true;
		Debug::LogWarn("LightLamp True");
	}
}
