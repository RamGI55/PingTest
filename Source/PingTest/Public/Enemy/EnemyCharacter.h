// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UAnimMontage; 
UCLASS()
class PINGTEST_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void GetNotification(AEnemyCharacter* PingedEnemy);
	// This function will be called when the player pings an enemy character 

	UFUNCTION(BlueprintCallable, Category = "Outline")
	void EnableOutline();

	UFUNCTION(BlueprintCallable, Category = "Outline")
	void DisableOutline(AEnemyCharacter* LostEnemy); 

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float Health = 100.0f;
	
};
