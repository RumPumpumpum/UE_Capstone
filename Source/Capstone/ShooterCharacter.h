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
	// Camera boom(ī�޶� ��)�� ĳ���� �ڿ� ī�޶� ��ġ ��Ų��.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	
	/* �����ڸ� ȣ������ �ʱ����� �����ͺ����� ����
	������ ������ ��ü�� �������� �ʰ� ��ü�� ���� ������ ������ �� �����Ƿ� �޸� ��뷮�� ����*/
	class USpringArmComponent* CameraBoom;
public:
	/*FORCEINLINE�� �Լ��� �ζ��� �Լ��� �����ϴ� ��ũ��
	�ַ� ���� �Լ��� ����� ȣ��Ǵ� �Լ��� �ζ��� �Լ��� �����Ͽ� ���α׷��� ������ ���*/

	// CameraBoom�� ��ȯ�Ѵ�.
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
