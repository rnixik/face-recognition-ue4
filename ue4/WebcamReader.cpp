// Fill out your copyright notice in the Description page of Project Settings.

#include "WebcamReader.h"
#include "Engine.h"


// Sets default values
AWebcamReader::AWebcamReader()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraID = 0;
	isStreamOpen = false;
	CameraUpdateFrameRate = 30.0f;
	VideoSize = FVector2D(1280, 720);
	buffer.reserve(1920 * 1080 * 3);

#ifdef __clang__
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif
	
}


FString AWebcamReader::GetClassifierFilePath()
{
	FString ClassifierFilePath = FPaths::Combine(*FPaths::GameContentDir(), *FString("DataDir/lbpcascade_frontalface.xml"));

	// https://forums.unrealengine.com/showthread.php?102830-4-10-Android-package-custom-files-along
#if PLATFORM_ANDROID
	extern FString GExternalFilePath;

	// read the file contents and write it if successful to external path
	TArray<uint8> MemFile;
	if (FFileHelper::LoadFileToArray(MemFile, *ClassifierFilePath, 0))
	{
		FString DestFilename = GExternalFilePath / FPaths::GetCleanFilename(*ClassifierFilePath);
		FFileHelper::SaveArrayToFile(MemFile, *DestFilename);
		ClassifierFilePath = DestFilename;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Can not read file contents from UFS to copy it on external storage"));
	}
#endif

	return ClassifierFilePath;
}

// Called when the game starts or when spawned
void AWebcamReader::BeginPlay()
{
	Super::BeginPlay();

	VideoTexture = UTexture2D::CreateTransient(VideoSize.X, VideoSize.Y);
	VideoTexture->UpdateResource();
	bGotActualVideoSize = false;

	faceRecognition = new FaceRecognition();

	FString ClassifierFilePath = GetClassifierFilePath();
	bool loadResult = faceRecognition->loadClassifierFile(TCHAR_TO_UTF8(*ClassifierFilePath));
	if (!loadResult) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Can not load classifier file: ") + ClassifierFilePath);
	}

	isStreamOpen = faceRecognition->captureCamera(CameraID, VideoSize.X, VideoSize.Y);
}

void AWebcamReader::UpdateActualVideoSize()
{
	if (isStreamOpen)
	{
		int width, height;
		bool sizeResult = faceRecognition->getActualVideoSize(width, height);
		if (!sizeResult) {
			return;
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Actual video size is ") + FString::FromInt(width) + TEXT("x") + FString::FromInt(height));
		ActualVideoSize = FVector2D(width, height);

		VideoTexture = UTexture2D::CreateTransient(ActualVideoSize.X, ActualVideoSize.Y);
		VideoTexture->UpdateResource();

		//Initialize data array
		Data.Init(FColor(0, 0, 0, 255), ActualVideoSize.X * ActualVideoSize.Y);

		bGotActualVideoSize = true;
	}
}

void AWebcamReader::UpdateTextureSlow()
{
		if (isStreamOpen && buffer.size() && VideoTexture)
		{
			//Copy Mat data to Data array
			for (int y = 0; y < ActualVideoSize.Y; y++)
			{
				for (int x = 0; x < ActualVideoSize.X; x++)
				{
					int i = x + (y * ActualVideoSize.X);
					Data[i].B = buffer[i * 3 + 0];
					Data[i].G = buffer[i * 3 + 1];
					Data[i].R = buffer[i * 3 + 2];
				}
			}

			FTexture2DMipMap& Mip = VideoTexture->PlatformData->Mips[0];
			void* TextureData = Mip.BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, (uint8*)Data.GetData(), 4 * Data.Num());
			Mip.BulkData.Unlock();
			VideoTexture->UpdateResource();
		}
}


// Called every frame
void AWebcamReader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FrameUpdateTime += DeltaTime;

	if (isStreamOpen && FrameUpdateTime > 1 / CameraUpdateFrameRate)
	{
		if (!bGotActualVideoSize) {
			UpdateActualVideoSize();
		}
		if (bGotActualVideoSize) {
			UpdateFrame();
			UpdateTextureSlow();
			OnNextVideoFrame();
		}
		FrameUpdateTime = 0.0f;
	}

}

void AWebcamReader::UpdateFrame()
{
	if (isStreamOpen)
	{
		faceRecognition->update(buffer);

		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::FromInt(usedMarkers));
	}
}

void AWebcamReader::OnNextVideoFrame_Implementation()
{
	// No default implementation
}


void AWebcamReader::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (faceRecognition) {
		faceRecognition->releaseCamera();
		delete faceRecognition;
	}
	
	isStreamOpen = false;
}
