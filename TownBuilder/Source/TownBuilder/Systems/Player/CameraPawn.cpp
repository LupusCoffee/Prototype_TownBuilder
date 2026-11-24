#include "Systems/Player/CameraPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACameraPawn::ACameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(SceneRoot);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	SpringArm->SetRelativeRotation(FRotator(-50.f, 0.f, 0.f));
	SpringArm->TargetArmLength = 2000.f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.f;
}

void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();

	SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength, MinZoom, MaxZoom);
	TargetZoom = SpringArm->TargetArmLength;
}

void ACameraPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetZoom, DeltaSeconds, ZoomSmoothing);
}

void ACameraPawn::Move(FVector Axis)
{
	const FRotator YawOnly(0.f, GetActorRotation().Yaw, 0.f);
	const FRotationMatrix YawMatrix(YawOnly);

	const FVector Direction = YawMatrix.GetUnitAxis(EAxis::X) * Axis.Y + YawMatrix.GetUnitAxis(EAxis::Y) * Axis.X;

	const float ZoomScale = FMath::GetMappedRangeValueClamped(
		FVector2D(MinZoom, MaxZoom), 
		FVector2D(MinZoomPanScale, MaxZoomPanScale),
		SpringArm->TargetArmLength
		);
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();

	AddActorWorldOffset(Direction * MoveSpeed * ZoomScale * DeltaSeconds + FVector::UpVector * Axis.Z * VerticalSpeed * DeltaSeconds);
}

void ACameraPawn::Zoom(float Axis)
{
	TargetZoom = FMath::Clamp(TargetZoom - Axis * ZoomStep, MinZoom, MaxZoom);
}

void ACameraPawn::Rotate(FVector2D Axis)
{
	AddActorWorldRotation(FRotator(0.f, Axis.X * RotateSensitivity, 0.f));

	FRotator ArmRotation = SpringArm->GetRelativeRotation();
	ArmRotation.Pitch = FMath::Clamp(ArmRotation.Pitch + Axis.Y * RotateSensitivity, MinPitch, MaxPitch);
	SpringArm->SetRelativeRotation(ArmRotation);
}
