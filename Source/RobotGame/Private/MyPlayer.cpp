// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayer.h"

namespace
{
	static const int MAX_ROLL = 30;
}

// Sets default values
AMyPlayer::AMyPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	check(m_SpringArmComp != nullptr);

	m_CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(m_CameraComponent != nullptr);

	// Attach the camera component to our capsule component.
	m_SpringArmComp->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	m_SpringArmComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
	m_SpringArmComp->TargetArmLength = 400.f;
	m_SpringArmComp->bEnableCameraLag = true;
	m_SpringArmComp->CameraLagSpeed = 3.0f;

	m_CameraComponent->SetupAttachment(m_SpringArmComp, USpringArmComponent::SocketName);

	m_SpringArmComp->bUsePawnControlRotation = false;

}

// Called when the game starts or when spawned
void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Using MyPlayer"));
	
}

// Called every frame
void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveForward(1.f);
	MoveRight();

	//LockAngleYaw();
}

// Called to bind functionality to input
void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAxis("MoveForward", this, &AMyPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyPlayer::Tilt);
	PlayerInputComponent->BindAxis("FloatUp", this, &AMyPlayer::FloatUp);

	//PlayerInputComponent->BindAxis("Turn", this, &AMyPlayer::CameraTurn);
	//PlayerInputComponent->BindAxis("LookUp", this, &AMyPlayer::CameraLookUp);
}

void AMyPlayer::MoveForward(float Value)
{
	if (Value == 0.f)return;

	FVector direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
	direction.Z = 0.0f; // z軸成分を0に設定
	AddMovementInput(direction.GetSafeNormal(), Value);
	//UE_LOG(LogTemp, Warning, TEXT("\n%f"), direction.X);
}

void AMyPlayer::MoveRight()
{
	static const float MAX_VELOCITY_Y = 1000.f;
	float roll = GetMesh()->GetRelativeRotation().Roll;
	float turnValue = roll / MAX_ROLL;
	
	//UE_LOG(LogTemp, Warning, TEXT("\n%f"), GetCharacterMovement()->Velocity.Y);
	

	if (roll < 1.f && roll > -1.f)
	{	
		return;
	}

	FVector direction = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::Y);
	direction.Z = 0.0f; // z軸成分を0に設定
	AddMovementInput(direction.GetSafeNormal(), turnValue);
}

void AMyPlayer::Tilt(float Value)
{
	static const float RETURN_SPEED = 0.8f;
	const float ROLL_SPEED = 0.5f * Value;


	if (Value == 0.f)
	{
		float nowPitch = GetMesh()->GetRelativeRotation().Pitch;
		FRotator TargetRotation = { nowPitch,0,0 };

		// 現在の回転を取得
		FRotator CurrentRotation = GetMesh()->GetRelativeRotation();

		// 補間率を計算（DeltaTimeはフレーム時間）
		static const float INTERP_SPEED = 2.f;
		const float DELTA_TIME = GetWorld()->GetDeltaSeconds();

		// 補間を行い、メッシュの回転を更新
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DELTA_TIME, INTERP_SPEED);
		GetMesh()->SetRelativeRotation(NewRotation);
		return;
	}


	if (Value > 0)
	{
		GetMesh()->AddRelativeRotation(FRotator(0, 0, ROLL_SPEED));

		FRotator rot = GetMesh()->GetRelativeRotation();
		if (rot.Roll > MAX_ROLL)
		{
			GetMesh()->SetRelativeRotation(FRotator(rot.Pitch, rot.Yaw, MAX_ROLL));
			SetAngleYaw(true);
		}
	}
	else
	{
		GetMesh()->AddLocalRotation(FRotator(0, 0, ROLL_SPEED));

		FRotator rot = GetMesh()->GetRelativeRotation();
		if (rot.Roll < -MAX_ROLL)
		{
			GetMesh()->SetRelativeRotation(FRotator(rot.Pitch, rot.Yaw, -MAX_ROLL));
			SetAngleYaw(false);
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("\n%f"), GetMesh()->GetRelativeRotation().Yaw);
}

void AMyPlayer::FloatUp(float Value)
{
	static const int MAX_PITCH_VALUE = 10;
	static const float RETURN_SPEED = 0.2f;
	const float PITCH_SPEED = 0.1f * Value;


	if (Value == 0.f)
	{
		float nowYaw = GetMesh()->GetRelativeRotation().Yaw;
		float nowRoll = GetMesh()->GetRelativeRotation().Roll;
		FRotator TargetRotation = { 0,nowYaw,nowRoll };

		// 現在の回転を取得
		FRotator CurrentRotation = GetMesh()->GetRelativeRotation();

		// 補間率を計算（DeltaTimeはフレーム時間）
		static const float INTERP_SPEED = 2.f;
		const float DELTA_TIME = GetWorld()->GetDeltaSeconds();

		// 補間を行い、メッシュの回転を更新
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DELTA_TIME, INTERP_SPEED);
		GetMesh()->SetRelativeRotation(NewRotation);
		
		return;
	}


	if (Value > 0)
	{
		GetMesh()->AddRelativeRotation(FRotator(PITCH_SPEED, 0, 0));

		FRotator rot = GetMesh()->GetRelativeRotation();
		if (rot.Pitch > MAX_PITCH_VALUE)
		{
			GetMesh()->SetRelativeRotation(FRotator(MAX_PITCH_VALUE, rot.Yaw, rot.Roll));
		}
	}
	else
	{
		GetMesh()->AddRelativeRotation(FRotator(PITCH_SPEED, 0, 0));

		FRotator rot = GetMesh()->GetRelativeRotation();
		if (rot.Pitch < -MAX_PITCH_VALUE)
		{
			GetMesh()->SetRelativeRotation(FRotator(-MAX_PITCH_VALUE, rot.Yaw, rot.Roll));
		}
	}
}

void AMyPlayer::CameraTurn(float Val)
{
	FRotator rot = m_SpringArmComp->GetRelativeRotation();

	if (Val != 0.f)
	{
		m_SpringArmComp->AddRelativeRotation(FRotator(0, Val, 0));
		
		//static const float MAX_TURN = 40.f;

		//if (rot.Yaw >= MAX_TURN)m_SpringArmComp->SetRelativeRotation(FRotator(rot.Pitch, MAX_TURN, rot.Roll));
		//if (rot.Yaw <= -MAX_TURN)m_SpringArmComp->SetRelativeRotation(FRotator(rot.Pitch, -MAX_TURN, rot.Roll));

		//UE_LOG(LogTemp, Warning, TEXT("\n%f"), m_SpringArmComp->GetRelativeRotation().Yaw);
	}
}

void AMyPlayer::CameraLookUp(float Val)
{
	if (Val != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		m_SpringArmComp->AddRelativeRotation(FRotator(Val, 0, 0));

		const float LOOK_UP_MAX = 35.f;
		const float LOOK_UP_MIN = -50.f;

		FRotator rot = m_SpringArmComp->GetRelativeRotation();
		if (rot.Pitch <= LOOK_UP_MIN)m_SpringArmComp->SetRelativeRotation(FRotator(LOOK_UP_MIN, rot.Yaw, rot.Roll));
		if (rot.Pitch >= LOOK_UP_MAX)m_SpringArmComp->SetRelativeRotation(FRotator(LOOK_UP_MAX, rot.Yaw, rot.Roll));
	}
}

void AMyPlayer::LockAngleYaw()
{
	FRotator rot = GetMesh()->GetRelativeRotation();
	if (rot.Yaw != 0.f)
	{
		GetMesh()->SetRelativeRotation(FRotator(rot.Pitch, 0, rot.Roll));
	}
}

void AMyPlayer::SetAngleYaw(bool isPositive)
{
	float YAW_SPEED = 0.1f;
	float MAX_YAW = 15.f;
	if (!isPositive)
	{
		YAW_SPEED *= -1;
		MAX_YAW *= -1;
	}

	GetMesh()->AddRelativeRotation(FRotator(0, YAW_SPEED, 0));
	FRotator rot = GetMesh()->GetRelativeRotation();
		
	if (isPositive)
	{
		if (rot.Yaw > MAX_YAW)
		{
			GetMesh()->SetRelativeRotation(FRotator(rot.Pitch, MAX_YAW, rot.Roll));
		}
	}
	else
	{
		if (rot.Yaw < MAX_YAW)
		{
			GetMesh()->SetRelativeRotation(FRotator(rot.Pitch, MAX_YAW, rot.Roll));
		}
	}

}

