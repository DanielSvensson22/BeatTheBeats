// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/SettingsManager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Serialization/Archive.h"


// Sets default values
ASettingsManager::ASettingsManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASettingsManager::BeginPlay()
{
	Super::BeginPlay();
	GetSaveFilePath();
}

// Called every frame
void ASettingsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASettingsManager::SetMusicVolume(float newMusic)
{
	MusicVolume = newMusic;
}

void ASettingsManager::SetEffectsVolume(float newEffects)
{
	EffectsVolume = newEffects;
}

void ASettingsManager::SetCameraShake(bool newShake)
{
	CameraShake = newShake;
}

//Only call from BeginPlay
void ASettingsManager::GetSaveFilePath()
{
	IPlatformFile& _fileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString _dir = FPaths::ProjectConfigDir();
	_dir.Append(TEXT("Save"));

	//If directory doesn't exist, create directory
	if (!FPaths::DirectoryExists(_dir)) _fileManager.CreateDirectory(*_dir);

	//If save file doesn't exist, create save file
	_dir.Append(TEXT("/Settings.cfg"));

	if (!_fileManager.FileExists(*_dir)) 
	{
		FFileHelper::SaveStringToFile((FString)"", *_dir, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		FilePath = _dir;
		MusicVolume = 1;
		EffectsVolume = 1;
		CameraShake = 1;
		SaveToFile();
	}
	FilePath = _dir;
}

//Call when exiting settings and if no file was detected in get file path
void ASettingsManager::SaveToFile()
{
	FString _settingsText = FString::Printf(TEXT("MusicVolume=%2f,\nEffectsVolume=%2f,\nCameraShake=%d"), MusicVolume, EffectsVolume, CameraShake);
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FString FileContent;
	if (FileManager.FileExists(*FilePath))
	{
		FFileHelper::SaveStringToFile(_settingsText, *FilePath, FFileHelper::EEncodingOptions::AutoDetect);
	}
	else UE_LOG(LogTemp, Warning, TEXT("Settings file not found"));
}

//Call when entering settings
void ASettingsManager::UpdateSettings()
{
	ReadFromFile();
	TArray<FString> Out;
	Settings.ParseIntoArray(Out, TEXT(",\n"), true);
	for (int i = 0; i < Out.Num(); i++)
	{
		TArray<FString> sub;
		Out[i].ParseIntoArray(sub, TEXT("="), true);
		if (sub[0] == "MusicVolume")
		{

			UE_LOG(LogTemp, Warning, TEXT("Parsing MusicVolume"));
			if (sub[1].IsNumeric()) MusicVolume = FCString::Atof(*sub[1]);
		}
		else if (sub[0] == "EffectsVolume")
		{

			UE_LOG(LogTemp, Warning, TEXT("Parsing EffectsVolume"));
			if (sub[1].IsNumeric()) EffectsVolume = FCString::Atof(*sub[1]);
		}
		else if (sub[0] == "CameraShake")
		{

			UE_LOG(LogTemp, Warning, TEXT("Parsing CameraShake"));
			if (sub[1].IsNumeric()) CameraShake = sub[1].ToBool();
		}
		else UE_LOG(LogTemp, Warning, TEXT("Could not parse"));
	}
}

//Only call from UpdateSettings
void ASettingsManager::ReadFromFile()
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FString FileContent;
	if (FileManager.FileExists(*FilePath))
	{
		if (FFileHelper::LoadFileToString(Settings, *FilePath, FFileHelper::EHashOptions::None))
		{
			UE_LOG(LogTemp, Warning, TEXT("Settings successfully loaded"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Did not load text from file"));
		}
	}
	else UE_LOG(LogTemp, Warning, TEXT("Settings file not found"));
}