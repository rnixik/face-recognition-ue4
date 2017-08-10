// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FaceRecognition.h"
#include <array>
#include <vector>
#include "WebcamReader.generated.h"

UCLASS()
class FACERECOGNITIONDEMO_API AWebcamReader : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWebcamReader();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// The device ID opened by the Video Stream
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
		int32 CameraID;

	// If the stream has succesfully opened yet
	UPROPERTY(BlueprintReadWrite, Category = Webcam)
		bool isStreamOpen;

	// The videos width and height (width, height)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
		FVector2D VideoSize;

	// The current video frame's corresponding texture
	UPROPERTY(BlueprintReadWrite, Category = Webcam)
		UTexture2D* VideoTexture;

	// The current data array
	UPROPERTY(BlueprintReadWrite, Category = Webcam)
		TArray<FColor> Data;

	// Blueprint Event called every time the video frame is updated
	UFUNCTION(BlueprintNativeEvent, Category = Webcam)
		void OnNextVideoFrame();

	// Limit camera update framerate
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Webcam)
		float CameraUpdateFrameRate;


protected:

	void UpdateFrame();
	void UpdateTextureSlow();

	FaceRecognition* faceRecognition;
	std::vector<uchar> buffer;

	FString GetClassifierFilePath();

	float FrameUpdateTime;

	FVector2D ActualVideoSize;
	bool bGotActualVideoSize;
	void UpdateActualVideoSize();
};
