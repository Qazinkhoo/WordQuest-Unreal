#include "WordQuestSwordPickup.h"
#include "WordQuestCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AWordQuestSwordPickup::AWordQuestSwordPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    SetRootComponent(Trigger);
    Trigger->SetBoxExtent(FVector(70.f, 70.f, 120.f));
    Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

    Blade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Blade"));
    Blade->SetupAttachment(Trigger);
    Blade->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (CubeMesh.Succeeded()) Blade->SetStaticMesh(CubeMesh.Object);
    Blade->SetRelativeScale3D(FVector(0.12f, 0.12f, 0.85f));
    Blade->SetRelativeLocation(FVector(0.f, 0.f, 65.f));

    Guard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Guard"));
    Guard->SetupAttachment(Trigger);
    Guard->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (CubeMesh.Succeeded()) Guard->SetStaticMesh(CubeMesh.Object);
    Guard->SetRelativeScale3D(FVector(0.55f, 0.16f, 0.10f));
    Guard->SetRelativeLocation(FVector(0.f, 0.f, -20.f));

    Handle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle"));
    Handle->SetupAttachment(Trigger);
    Handle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (CubeMesh.Succeeded()) Handle->SetStaticMesh(CubeMesh.Object);
    Handle->SetRelativeScale3D(FVector(0.16f, 0.16f, 0.45f));
    Handle->SetRelativeLocation(FVector(0.f, 0.f, -65.f));

    SetActorRotation(FRotator(0.f, 0.f, -25.f));
}

void AWordQuestSwordPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
    if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(OtherActor))
    {
        Player->CollectSword();
        Destroy();
    }
}
