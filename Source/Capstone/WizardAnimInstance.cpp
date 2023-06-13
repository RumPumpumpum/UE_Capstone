	// Fill out your copyright notice in the Description page of Project Settings.


#include "WizardAnimInstance.h"
#include "Enemy.h"

void UWizardAnimInstance::UpdateAnimationProperties(float Deltatime)
{
	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	if (Enemy)
	{
		FVector Velocity{ Enemy->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
	}
}
