// Fill out your copyright notice in the Description page of Project Settings.


#include "Article.h"
#include "Player/Scientist.h"
#include "Player/ScientistController.h"


#include <Components/BoxComponent.h>
#include "GameHudWidget.h"
#include "Debug.h"
#include <Kismet/GameplayStatics.h>

AArticle::AArticle()
	: Title("The Title of the article")
	, Content("Content")
{
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionVolume"));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	TriggerBox->SetVisibility(false);

	SetRootComponent(TriggerBox);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(TriggerBox);
}

void AArticle::PickUP(AScientist* mainPlayer)
{
	Debug::LogWarn("Picked Up");
	ABaseItem::PickUP(mainPlayer);

	if (mainPlayer == nullptr)
		return;

	SetActorHiddenInGame(true);
	mainPlayer->GetInventory().CollectArticle(this);
	
	if (TriggerBox)
		TriggerBox->DestroyComponent();

	if (Mesh)
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UGameHudWidget::Get()->AddDialogText(FText::FromString("I"), FText::FromString("Artikel im Inventar lesen"));
	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, [this]() { UGameHudWidget::Get()->ClearDialogText(); }, 2.0f, false);

	IsPaperOpend = false;
	//OpenPaper();
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Newspaper, GetActorLocation());
}

void AArticle::OpenPaper()
{
	AScientist* player = Cast<AScientist>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (player)
	{
		FVector cameraLocation;
		FRotator cameraRotation;

		player->GetPlayerController()->GetPlayerViewPoint(cameraLocation, cameraRotation);

		Mesh->SetWorldLocation(cameraLocation + cameraRotation.Vector() * m_DistanceFromCamera);
		FRotator PaperRotation = (cameraLocation - Mesh->GetRelativeLocation()).Rotation();
		PaperRotation.Pitch -= 90.0f;

		Mesh->SetRelativeRotation(PaperRotation);

		float distance = FVector::Dist(Mesh->GetRelativeLocation(), cameraLocation);
		player->GetPlayerController()->EnableDepthOfFieldOnObject(player->GetFirstPersonCameraComponent(), distance);

		IsPaperOpend = true;
	}
}

void AArticle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (IsPaperOpend)
	{
		AScientist* player = Cast<AScientist>(UGameplayStatics::GetPlayerPawn(this, 0));
		if (player && player->GetPlayerController()->WasInputKeyJustPressed(EKeys::Q))
		{
			SetActorHiddenInGame(true);
			player->GetInventory().CollectArticle(this);
			TriggerBox->DestroyComponent();
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			IsPaperOpend = false;
			player->GetPlayerController()->DisableDepthOfField(player->GetFirstPersonCameraComponent());
		}
	}*/
}

void AArticle::OverlapBeginn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto player = Cast<AScientist>(OtherActor);
	if (player)
	{
		UGameHudWidget::Get()->AddDialogText(FText::FromString("E"), FText::FromString("Nehmen"));
	}
}

void AArticle::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AScientist>(OtherActor))
	{
		UGameHudWidget::Get()->ClearDialogText();
	}
}

void AArticle::BeginPlay()
{
	Super::BeginPlay();

	//TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AArticle::OverlapBeginn);
	//TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AArticle::OverlapEnd);

	IsPaperOpend = false;
}
