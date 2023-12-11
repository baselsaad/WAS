// Copyright Epic Games, Inc. All Rights Reserved.


#include "WASGameModeBase.h"

#include "Player/Scientist.h"
#include "Player/ScientistController.h"

AWASGameModeBase::AWASGameModeBase()
	: Super()
{

	// set default pawn class to our Blueprinted character
	ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Player/Scientist_BP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	ConstructorHelpers::FClassFinder<AScientistController> PlayerControllerClassFinder(TEXT("/Game/Blueprints/Player/ScientistController_BP"));
	PlayerControllerClass = PlayerControllerClassFinder.Class;
}
