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
	InitComponent();  // �� This adds the mapping context
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

void UTP_PingComponent::PingWithValue(const FInputActionValue& Value)
{
	Ping();
}


void UTP_PingComponent::Ping()
{
	LineTraceForPing();
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Ping")); 
	}
}

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

		TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); 
		
		UWorld* World = GetWorld();
		
		bool bHit  = World->SweepSingleByObjectType(
			HitResult,
			CameraLocation,
			TraceEnd,
			FQuat::Identity,
			ObjectTypes,
			FCollisionShape::MakeSphere(20.f),
			QueryParams
			);
		
		if (bHit)
		{
			// Hit something
			PingLocation = HitResult.Location;
        
			// Debug draw
			DrawDebugSphere(World, PingLocation, PingRadius, 12, FColor::Red, false, 3.0f);
        
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
			DrawDebugSphere(World, PingLocation, PingRadius, 12, FColor::Blue, false, 3.0f);
        
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Pinged: Empty Space"));
			}
		}
#if ENABLE_DRAW_DEBUG
	FVector CapsuleOrigin = CameraLocation + (TraceEnd - CameraLocation) * 0.5f;
		FVector EndLocation = bHit ? HitResult.Location : TraceEnd; 
	FColor CapsuleColor = bHit ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, 50.f, 20.f,FQuat::Identity, CapsuleColor, false, 3.0f);
		DrawDebugLine(World, CameraLocation, EndLocation, FColor::White, false, 3.0f, 0, 1.0f);
#endif 
	}
}


