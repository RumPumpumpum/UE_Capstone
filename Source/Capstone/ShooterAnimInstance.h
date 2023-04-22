// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONE_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	
	/* �ִϸ��̼� �Ӽ��� ������Ʈ */
	void UpdateAnimationProperties(float Deltatime);
	
	/** �ִϸ��̼� �������(����,�Ӽ�) �ʱ�ȭ */
	virtual void NativeInitializeAnimation() override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = " true"));
	class AShooterCharacter* ShooterCharacter;

	/** ĳ������ �ӵ� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = " true"))
	float Speed;
	
	/** ĳ���Ͱ� ���߿� �ִ°� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = " true"))
	bool bIsInAir; // b= boolean
	
	/** ĳ���Ͱ� �����̴� ������ �ƴ��� */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = " true"))
	bool bIsAccelerating;

	/** �̵���ݿ� ���Ǵ� Yaw ������*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = " true"))
	float MovementOffsetYaw;


}; 
