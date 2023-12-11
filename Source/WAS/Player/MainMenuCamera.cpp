// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MainMenuCamera.h"
#include "Camera/CameraComponent.h"

float Iterator = 0.0f;

// Sets default values
AMainMenuCamera::AMainMenuCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));

	MainCamera->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMainMenuCamera::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->SetViewTarget(this);
}

// Called every frame
void AMainMenuCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewRotation = FRotator(0.0f, FMath::Sin(Iterator)/20, 0.0f);
	MainCamera->AddLocalRotation(NewRotation);
	Iterator += .5f * DeltaTime;
}

