#include "Boss/Bullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Boss\Boss.h"

ABullet::ABullet()
{
	// Set default values for the bullet actor
	PrimaryActorTick.bCanEverTick = true;

	// Create the collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

	// Create the projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bAutoActivate = false;  // Don't start movement immediately
	ProjectileMovement->SetUpdatedComponent(CollisionComponent);
	ProjectileMovement->InitialSpeed = 1500.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0;

	// Set the overlap event for the collision component
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlapBegin);
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	// Start the bullet's movement
	ProjectileMovement->Activate();
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	// Check if the player reference exists and if we're homing
	if (Player)
	{
		float DistanceToPlayer = (Player->GetActorLocation() - GetActorLocation()).Size();

		TravelTime -= DeltaTime;
		if (TravelTime <= 0.001f) TravelTime = 0.001f;
		// Calculate the speed based on distance and time
		float Speed = DistanceToPlayer / TravelTime;

		// Set the initial velocity of the bullet towards the player
		FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		ProjectileMovement->Velocity = Direction * Speed;

		// Interpolate the velocity towards the target
		FVector TargetVelocity = Direction * ProjectileMovement->Velocity.Size();
		ProjectileMovement->Velocity = FMath::VInterpTo(ProjectileMovement->Velocity, TargetVelocity, DeltaTime, 10.0f);

		// Optional: Rotate the bullet towards the direction of travel
		FRotator NewRotation = Direction.Rotation();
		SetActorRotation(FQuat::Slerp(GetActorQuat(), NewRotation.Quaternion(), DeltaTime * 5.0f));
	}
}

void ABullet::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor == Player)
	{
		// Apply damage through the boss event
		Boss->ApplyBulletDamage();

		// Spawn hit effect
		if (HitEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, GetActorLocation());
		}

		// Destroy the bullet after impact
		Destroy();
	}
}