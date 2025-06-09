// Fill out your copyright notice in the Description page of Project Settings.


#include "PingSystem/TP_PingComponent.h"
#include "PingTest/Public/PingSystem/TP_PingComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PingTest/PingTestCharacter.h"
#include "InputAction.h"
#include "InputMappingContext.h" 
#include "InputActionValue.h"


// Sets default values for this component's properties
UTP_PingComponent::UTP_PingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
	PingAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/FirstPerson/Input/Actions/IA_Ping.IA_Ping"));
	InputContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/FirstPerson/Input/IMC_Pings.IMC_Pings"));
	
}

void UTP_PingComponent::InitComponent()
{
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (InputContext)
			{
				Subsystem->AddMappingContext(InputContext, 1);
			}
		}
	}
	
}

void UTP_PingComponent::BeginPlay()
{
	Super::BeginPlay();
	InitComponent();  // ก็ This adds the mapping context
}

void UTP_PingComponent::SetupInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (PingAction)
		{
			EnhancedInputComponent->BindAction(PingAction, ETriggerEvent::Triggered, this, &UTP_PingComponent::Ping);
		}
	} 
}


void UTP_PingComponent::Ping()
{
	UE_LOG(LogTemp, Warning, TEXT("Component ping activated!"));
    
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Ping from Component!"));
	}
}


