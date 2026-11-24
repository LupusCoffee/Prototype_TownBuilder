#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class TOWNBUILDER_API ACameraPawn : public APawn
{
	GENERATED_BODY()


public:
	ACameraPawn();

	//Methods --> Controlling Camera
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void Move(FVector Axis);
	
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void Zoom(float Axis);
	
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void Rotate(FVector2D Axis);


protected:
	//Setup & Tick
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	
	//Static Variables --> Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;


	//Static Variables --> Editable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Movement")
	float MoveSpeed = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Movement")
	float VerticalSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Movement")
	float MinZoomPanScale = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Movement")
	float MaxZoomPanScale = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Rotation")
	float RotateSensitivity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Rotation")
	float MinPitch = -80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Rotation")
	float MaxPitch = -15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
	float ZoomStep = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
	float MinZoom = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
	float MaxZoom = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Zoom")
	float ZoomSmoothing = 8.f;


private:
	//Dynamic Variables
	float TargetZoom = 0.f;
};
