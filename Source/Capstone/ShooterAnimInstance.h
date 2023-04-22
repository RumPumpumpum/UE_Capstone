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
	
	/* 애니메이션 속성값 업데이트 */
	void UpdateAnimationProperties(float Deltatime);
	
	/** 애니메이션 구성요소(변수,속성) 초기화 */
	virtual void NativeInitializeAnimation() override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = " true"));
	class AShooterCharacter* ShooterCharacter;

	/** 캐릭터의 속도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = " true"))
	float Speed;
	
	/** 캐릭터가 공중에 있는가 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = " true"))
	bool bIsInAir; // b= boolean
	
	/** 캐릭터가 움직이는 중인지 아닌지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = " true"))
	bool bIsAccelerating;

	/** 이동사격에 사용되는 Yaw 오프셋*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = " true"))
	float MovementOffsetYaw;


}; 
