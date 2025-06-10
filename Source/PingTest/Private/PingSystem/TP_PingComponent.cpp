// Fill out your copyright notice in the Description page of Project Settings.


#include "PingSystem/TP_PingComponent.h"
#include "PingTest/Public/PingSystem/TP_PingComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PingTest/PingTestCharacter.h"
#include "InputAction.h"
#include "InputMappingContext.h" 
#include "InputActionValue.h"
#include "Enemy/EnemyCharacter.h"

// Need the delegate for the bhit - send to the enemy character to enable outline. 

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
	InitComponent();  // ← This adds the mapping context
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

void UTP_PingComponent::StartDynamicPing()
{
	
}

void UTP_PingComponent::StopDynamicPing()
{
}

void UTP_PingComponent::UpdateDynamicPing()
{
}

void UTP_PingComponent::PingWithValue(const FInputActionValue& Value)
{
	Ping(); 
}


void UTP_PingComponent::Ping()
{
	LineTraceForPing(); 
	
}


// Using the linetracing to pint the enemy characters. 

void UTP_PingComponent::LineTraceForPing()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	if (PlayerController)
	{
		// use the Capsule ping the actor. 
		FVector CameraLocation;
		FRotator CameraRotation;
		float PingRange = 10000.0f; 
		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * PingRange);

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner()); // Ignore the player
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
		

		bool bHit = GetWorld()->LineTraceSingleByObjectType(
			HitResult,
			CameraLocation,
			TraceEnd,
			ObjectQueryParams,  // This is the key parameter that makes this method different
			QueryParams
		);
		
		if (bHit)
		{
			// Hit something
			PingLocation = HitResult.Location;
			// Get the actor hit however, didn't get the any enemy character information and won't able to send the delegate to the enemy character.
			
			if (AEnemyCharacter* HitEnemy = Cast<AEnemyCharacter>(HitResult.GetActor()))
			{
				OnEnemyPinged.Broadcast(HitEnemy);

				// Debug draw
				DrawDebugSphere(GetWorld(), PingLocation, PingRadius, 12, FColor::Red, false, 3.0f);
				
			}
        
			// Screen message
			if (GEngine)
			{
				FString HitActorName = HitResult.GetActor() ? HitResult.GetActor()->GetName() : TEXT("Unknown");
				FString PingMessage = FString::Printf(TEXT("Pinged: %s"), *HitActorName);
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, PingMessage);
				
			}
		}
		else
		{
			// No hit - ping at max range
			PingLocation = TraceEnd;
        
			// Debug draw
			DrawDebugSphere(GetWorld(), PingLocation, PingRadius, 12, FColor::Blue, false, 3.0f);
        
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Pinged: Empty Space"));
			}
		}

		// Optional: Draw debug line
		DrawDebugLine(GetWorld(), CameraLocation, PingLocation, FColor::Yellow, false, 1.0f, 0, 2.0f);
	}
}


