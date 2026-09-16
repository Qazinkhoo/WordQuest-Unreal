#include "WordQuestEnemy.h"
#include "WordQuestCharacter.h"
#include "WordQuestGameMode.h"
#include "WordQuestGameInstance.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AWordQuestEnemy::AWordQuestEnemy()
{
    PrimaryActorTick.bCanEverTick = false;

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    SetRootComponent(Trigger);
    Trigger->SetBoxExtent(FVector(70.f, 70.f, 90.f));
    Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
    Visual->SetupAttachment(Trigger);
    Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded()) Visual->SetStaticMesh(CubeMesh.Object);
    Visual->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.1f));
    Visual->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
}

void AWordQuestEnemy::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
    if (!Cast<AWordQuestCharacter>(OtherActor)) return;

    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>())
    {
        if (WaveNumber != GI->PlayerState.Wave) return;
    }

    if (AWordQuestGameMode* GM = Cast<AWordQuestGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        GM->StartEncounter(this);
    }
}

void AWordQuestEnemy::ConfigureEnemy(int32 InWave, bool bInBoss)
{
    WaveNumber = InWave;
    bBoss = bInBoss;
    MaxHP = bBoss ? 3 : 1;
    CurrentHP = MaxHP;
    SetActorScale3D(bBoss ? FVector(1.5f) : FVector(1.f));
}

bool AWordQuestEnemy::ReceiveWordDamage(int32 DamageAmount)
{
    CurrentHP = FMath::Max(0, CurrentHP - FMath::Max(0, DamageAmount));
    return CurrentHP <= 0;
}

void AWordQuestEnemy::PlayHitPulse()
{
    if (!GetWorld()) return;

    const FVector BaseScale = bBoss ? FVector(1.5f) : FVector(1.f);
    SetActorScale3D(BaseScale * 1.12f);

    FTimerHandle ResetHandle;
    TWeakObjectPtr<AWordQuestEnemy> WeakSelf = this;
    GetWorld()->GetTimerManager().SetTimer(ResetHandle, [WeakSelf, BaseScale]()
    {
        if (WeakSelf.IsValid())
        {
            WeakSelf->SetActorScale3D(BaseScale);
        }
    }, 0.12f, false);
}
