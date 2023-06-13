// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WizardAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_API UWizardAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	/* �ִϸ��̼� �Ӽ��� ������Ʈ */
	void UpdateAnimationProperties(float Deltatime);

private:
	/** �̵� �ӵ� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AEnemy* Enemy;

};
