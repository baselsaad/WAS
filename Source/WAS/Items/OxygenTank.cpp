#include "OxygenTank.h"
#include "Items/MasK.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Scientist.h"
#include "ScientistController.h"
#include "Menu/GameHudWidget.h"
#include "Tools/Debug.h"

AOxygenTank::AOxygenTank()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBoxForInteraction = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxForInteraction"));
	CollisionBoxForInteraction->InitBoxExtent(FVector(5, 5, 5));
	RootComponent = CollisionBoxForInteraction;

	OxygenTank = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mask"));
	OxygenTank->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	OxygenTank->SetupAttachment(RootComponent);
}

void AOxygenTank::OverlapBeginn(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Player = Cast<AScientist>(OtherActor);
	if (Player)
	{
		if (UGameHudWidget::Get())
			UGameHudWidget::Get()->AddDialogText(FText::FromString("E"), FText::FromString("Fuellen"));
		return;
	}
}

void AOxygenTank::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Player = Cast<AScientist>(OtherActor);
	if (Player)
	{
		Player = nullptr;
		if (UGameHudWidget::Get())
			UGameHudWidget::Get()->ClearDialogText();
	}
}

void AOxygenTank::BeginPlay()
{
	Super::BeginPlay();

	CollisionBoxForInteraction->OnComponentBeginOverlap.AddDynamic(this, &AOxygenTank::OverlapBeginn);
	CollisionBoxForInteraction->OnComponentEndOverlap.AddDynamic(this, &AOxygenTank::OverlapEnd);
}

void AOxygenTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Player)
		return;

	if (Player->GetPlayerController()->WasInputKeyJustPressed(EKeys::E))
	{
		InteractWithMask(Player->GetMask());
	}
}

void AOxygenTank::InteractWithMask(AMask* Mask)
{
	if (Mask)
	{
		Mask->OxygenRefill();
	}
}


