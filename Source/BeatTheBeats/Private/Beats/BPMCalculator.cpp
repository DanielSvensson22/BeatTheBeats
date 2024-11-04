// Fill out your copyright notice in the Description page of Project Settings.

#include "Sound/SoundWave.h"
#include "Beats/BPMCalculator.h"

float UBPMCalculator::BPMDetector(USoundWave* SoundWave)
{
    if (!SoundWave || !SoundWave->GetResourceData()) {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load File"));
        return 0.0f;
    }

    const int16* SampleData = reinterpret_cast<const int16*>(SoundWave->GetResourceData());
    int32 PCMDataSize = SoundWave->GetResourceSize();
    int32 NumSamples = PCMDataSize / sizeof(int16);
    int32 SampleRate = SoundWave->GetSampleRateForCurrentPlatform();

    constexpr float MinInterval = 0.3f; // 200 BPM
    constexpr float MaxInterval = 2.0f;  // 30 BPM

    // Normalize audio levels
    float MaxSampleValue = 0.0f;
    for (int32 i = 0; i < NumSamples; i++) {
        MaxSampleValue = FMath::Max(MaxSampleValue, FMath::Abs(SampleData[i]));
    }

    float NormalizationFactor = MaxSampleValue > 0 ? 32767.0f / MaxSampleValue : 1.0f; // Normalize to 16-bit range

    TArray<float> NormalizedSamples;
    for (int32 i = 0; i < NumSamples; i++) {
        NormalizedSamples.Add(SampleData[i] * NormalizationFactor);
    }

    // Calculate RMS for adaptive threshold
    float RMS = 0.0f;
    for (int32 i = 0; i < NumSamples; i++) {
        RMS += FMath::Pow(NormalizedSamples[i], 2);
    }
    RMS = FMath::Sqrt(RMS / NumSamples);
    float PeakThreshold = RMS * 1.01f; // Tune this multiplier as needed

    // Enhanced Peak Detection
    TArray<int32> PeakPos;
    int32 SmoothingWindow = 5; // Smoothing window size

    for (int32 i = SmoothingWindow; i < NumSamples - SmoothingWindow; i++) {
        // Smoothing calculation
        int32 SmoothSample = 0;
        for (int32 w = -SmoothingWindow; w <= SmoothingWindow; w++) {
            SmoothSample += NormalizedSamples[i + w];
        }
        SmoothSample /= (2 * SmoothingWindow + 1);

        // Check for peaks
        if (SmoothSample > PeakThreshold && NormalizedSamples[i] > NormalizedSamples[i - 1] && NormalizedSamples[i] > NormalizedSamples[i + 1]) {
            PeakPos.Add(i);
        }
    }

    // Calculate intervals between peaks
    TArray<float> Intervals;
    for (int32 i = 1; i < PeakPos.Num(); i++) {
        float Interval = (PeakPos[i] - PeakPos[i - 1]) / static_cast<float>(SampleRate);
        if (Interval >= MinInterval && Interval <= MaxInterval) {
            Intervals.Add(Interval);
        }
    }

    // Calculate average interval to determine BPM
    float AvgInterval = 0.0f;
    if (Intervals.Num() > 0) {
        for (float Interval : Intervals) {
            AvgInterval += Interval;
        }
        AvgInterval /= Intervals.Num();
    }

    UE_LOG(LogTemp, Warning, TEXT("Intervals: %d"),Intervals.Num());

    float BPM = (AvgInterval > 0.0f) ? (60.0f / AvgInterval) : 0.0f;
    UE_LOG(LogTemp, Warning, TEXT("Detected BPM: %f"), BPM);
    return FMath::RoundToInt(BPM);

}

