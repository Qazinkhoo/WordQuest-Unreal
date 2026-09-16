#include "WordQuestEnemy.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AWordQuestEnemy::AWordQuestEnemy()
{
    PrimaryActorTick.bCanEverTick = false;
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    SetRootComponent(Trigger);
    Trigger->SetBoxExtent(FVector(70.f, 70.f, 70.f));
    Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
    Visual->SetupAttachment(Trigger);
    Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
