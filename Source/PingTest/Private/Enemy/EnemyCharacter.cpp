// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "PingTest/Public/Enemy/EnemyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "PingSystem/TP_PingComponent.h"
#include "PingTest/PingTestCharacter.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	
	

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
	
}

