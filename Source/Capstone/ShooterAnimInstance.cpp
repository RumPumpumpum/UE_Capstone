// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	
	if (ShooterCharacter)
	{
		// 속도벡터(velocity)에서 캐릭터의 속도 가져옴
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// 캐릭터가 공중에 있는가?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// 캐릭터가 가속중 인가?
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	// PawnOwner를 가져와서(실패시 null) AShooterCharacter로 형변환, 이후 ShooterCharacter변수에 할당
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
