// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	
	if (ShooterCharacter)
	{
		// �ӵ�����(velocity)���� ĳ������ �ӵ� ������
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// ĳ���Ͱ� ���߿� �ִ°�?
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// ĳ���Ͱ� ������ �ΰ�?
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
	// PawnOwner�� �����ͼ�(���н� null) AShooterCharacter�� ����ȯ, ���� ShooterCharacter������ �Ҵ�
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}