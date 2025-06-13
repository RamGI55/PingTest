// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "PingTest/Public/Enemy/EnemyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "PingSystem/TP_PingComponent.h"
#include "PingTest/PingTestCharacter.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{

	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyCharacter::SetSpotted(const AActor* Spotter, bool bSpotted)
{
	// Team logic, which team the enemy character is on.
	// For now, we will just use the bool to determine if the enemy is spotted or not. 
	if (bSpotted)
	{
		if (!isSpotted)
		{
			isSpotted = true;
			EnableOutline(); // Enable outline when spotted
			UE_LOG(LogTemp, Warning, TEXT("Enemy %s spotted by %s"), *GetName(), *Spotter->GetName());
		}
	}
	else 
	{
		if (isSpotted)
		{
			isSpotted = false;
			DisableOutline(this); // Disable outline when not spotted
			UE_LOG(LogTemp, Warning, TEXT("Enemy %s lost sight by %s"), *GetName(), *Spotter->GetName());
		}
	}
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(AEnemyCharacter, isSpotted, COND_None);

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (APingTestCharacter* PlayerCharacter = Cast<APingTestCharacter>(PC->GetPawn()))
			{
				if (UTP_PingComponent* PingComp = PlayerCharacter->FindComponentByClass<UTP_PingComponent>())
				{
					check(PingComp);
					PingComp->OnEnemyPinged.AddDynamic(this, &AEnemyCharacter::GetNotification);
					PingComp->OnEnemyLost.AddDynamic(this, &AEnemyCharacter::DisableOutline);
					// Bind the delegate to the GetNotification function
				}
			}
		}
	}
}

void AEnemyCharacter::GetNotification(AEnemyCharacter* PingedEnemy)
{
	if (PingedEnemy == this)
	{
		EnableOutline(); 
		// Optionally, you can also log or handle the ping notification here
		UE_LOG(LogTemp, Warning, TEXT("Pinged Enemy: %s"), *GetName());
	}
	
}

void AEnemyCharacter::EnableOutline()
{

	if (GetMesh())
	{
		GetMesh()->SetRenderCustomDepth(true);
		GetMesh()->SetCustomDepthStencilValue(2); // Set a custom depth stencil value for the outline
	}
	
}

void AEnemyCharacter::DisableOutline(AEnemyCharacter* LostEnemy)
{
	if (GetMesh())
	{
		GetMesh()->SetRenderCustomDepth(false);
		GetMesh()->SetCustomDepthStencilValue(0); // Reset the custom depth stencil value
	}
}

