// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/flashlight.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Player/Scientist.h"
#include "Tools/Debug.h"



Aflashlight:: Aflashlight()
{

	CollisionBoxForInteraction = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxForInteraction"));
	CollisionBoxForInteraction->InitBoxExtent(FVector(5, 5, 5));
	RootComponent = CollisionBoxForInteraction;

	flashlight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pill"));
	flashlight->SetRelativeLocation(FVector(10.0f, 5.0f, -5.0f));
	flashlight->SetupAttachment(RootComponent);
}

void Aflashlight::PickUP(AScientist* mainPlayer)
{
	if (mainPlayer == nullptr)
		return;

	ABaseItem::PickUP(mainPlayer);

	Onpickup(mainPlayer);
	// Dialog hir erstellen
	SetActorHiddenInGame(true);

}

void Aflashlight::Onpickup(AScientist* mainPlayer)
{
	// Dialog hir erstellen 


}
