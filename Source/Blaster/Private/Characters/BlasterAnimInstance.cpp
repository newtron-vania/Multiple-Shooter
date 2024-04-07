// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BlasterAnimInstance.h"
#include "Characters/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// 애니메이션 초기화
void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 소유자가 BlasterCharacter인지 확인하고 설정
	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

// 애니메이션 갱신
void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	// BlasterCharacter가 유효한지 확인
	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (BlasterCharacter == nullptr) return; // 유효하지 않으면 종료

	// 속도 및 상태 변수 설정
	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f; // 수직 속도 제외
	Speed = Velocity.Size(); // 속도 계산

	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling(); // 공중에 떠있는지 여부
	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false; // 가속 중인지 여부
	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped(); // 무기를 장착했는지 여부
	bIsCrouched = BlasterCharacter->bIsCrouched; // 캐릭터가 웅크렸는지 여부
	bAiming = BlasterCharacter->IsAiming(); // 조준 중인지 여부

	// 조준 시 이동 방향에 따른 YawOffset 계산
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	// 캐릭터 회전을 기반으로 한 Lean 값 계산
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.0f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	// 조준 시 Yaw 및 Pitch 값 설정
	AO_Yaw = BlasterCharacter->GetAO_Yaw();
	AO_Pitch = BlasterCharacter->GetAO_Pitch();
}