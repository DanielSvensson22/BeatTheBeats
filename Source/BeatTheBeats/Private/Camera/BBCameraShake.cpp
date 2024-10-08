// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/BBCameraShake.h"

UBBCameraShake::UBBCameraShake()
{
	//Effect time
	OscillationDuration = 0.35f;
	OscillationBlendInTime = 0.1f;
	OscillationBlendOutTime = 0.2f;

	//Rotation Pitch
	RotOscillation.Pitch.Amplitude = 3.0f;
	RotOscillation.Pitch.Frequency = 3.0f;
	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Pitch.Waveform = EOscillatorWaveform::PerlinNoise;
	
	//Rotation Yaw
	RotOscillation.Yaw.Amplitude = 3.0f;
	RotOscillation.Yaw.Frequency = 3.0f;
	RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Yaw.Waveform = EOscillatorWaveform::PerlinNoise;

	//Location X
	LocOscillation.X.Amplitude = 3.0f;
	LocOscillation.X.Frequency = 50.0f;
	LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.X.Waveform = EOscillatorWaveform::SineWave;
	
	//Location Y
	LocOscillation.Y.Amplitude = 3.0f;
	LocOscillation.Y.Frequency = 50.0f;
	LocOscillation.Y.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Y.Waveform = EOscillatorWaveform::SineWave;
	
	//Location Z
	LocOscillation.Z.Amplitude = 3.0f;
	LocOscillation.Z.Frequency = 50.0f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Z.Waveform = EOscillatorWaveform::SineWave;
}