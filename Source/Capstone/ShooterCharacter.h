// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class CAPSTONE_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Camera boom(카메라 팔)은 캐릭터 뒤에 카메라를 위치 시킨다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	
	/* 생성자를 호출하지 않기위해 포인터변수로 선언
	포인터 변수는 객체를 복사하지 않고 객체에 대한 참조를 유지할 수 있으므로 메모리 사용량도 감소*/
	class USpringArmComponent* CameraBoom;
public:
	/*FORCEINLINE은 함수를 인라인 함수로 선언하는 매크로
	주로 작은 함수나 빈번히 호출되는 함수를 인라인 함수로 선언하여 프로그램의 성능을 향상*/

	// CameraBoom을 반환한다.
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
