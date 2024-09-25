#pragma once

UENUM(BlueprintType)
enum class ECameraState : uint8
{
	ECS_FreeCamera UMETA(DisplayName = "FreeCamera"),
	ECS_LockCamera UMETA(DisplayName = "LockCamera")
};