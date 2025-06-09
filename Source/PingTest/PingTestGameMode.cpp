// Copyright Epic Games, Inc. All Rights Reserved.

#include "PingTestGameMode.h"
#include "PingTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

APingTestGameMode::APingTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
