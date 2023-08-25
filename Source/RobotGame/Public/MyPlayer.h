// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyPlayer.generated.h"

UCLASS()
class ROBOTGAME_API AMyPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	UFUNCTION()
		void MoveForward(float Value);

	void MoveRight();

	// Handles input for moving right and left.
	UFUNCTION()
		void Tilt(float Value);

	UFUNCTION()
		void FloatUp(float Value);

	UFUNCTION()
		void CameraTurn(float Val);

	UFUNCTION()
		void CameraLookUp(float Val);


	UPROPERTY(EditAnywhere)
		USpringArmComponent* m_SpringArmComp;

	UPROPERTY(VisibleAnywhere)
		UCameraComponent* m_CameraComponent;


	void LockAngleYaw();

	void SetAngleYaw(bool isPositive);
};
