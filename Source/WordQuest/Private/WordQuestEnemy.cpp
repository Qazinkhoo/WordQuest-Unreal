#include "WordQuestEnemy.h"
#include "WordQuestCharacter.h"
#include "WordQuestGameMode.h"
#include "WordQuestGameInstance.h"
#include "WordQuestEnemyWidget.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
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
    if (CubeMesh.Succeeded())
    {
        Visual->SetStaticMesh(CubeMesh.Object);
    }
    Visual->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.1f));
    Visual->SetRelativeLocation(FVector(0.f, 0.f, 80.f));

    EnemyWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyWidgetComponent"));
    EnemyWidgetComponent->SetupAttachment(Trigger);
    EnemyWidgetComponent->SetWidgetClass(UWordQuestEnemyWidget::StaticClass());
    EnemyWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    EnemyWidgetComponent->SetPivot(FVector2D(0.5f, 1.0f));
    EnemyWidgetComponent->SetRelativeLocation(FVector(0.f, -2.f, -100.f));
    EnemyWidgetComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    EnemyWidgetComponent->SetTwoSided(true);
    EnemyWidgetComponent->SetBlendMode(EWidgetBlendMode::Transparent);
    EnemyWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EnemyWidgetComponent->SetDrawSize(FVector2D(150.f, 240.f));
}

void AWordQuestEnemy::BeginPlay()
{
    Super::BeginPlay();

    if (EnemyWidgetComponent)
    {
        EnemyWidget = Cast<UWordQuestEnemyWidget>(EnemyWidgetComponent->GetUserWidgetObject());
    }

    RefreshEnemyVisual();
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

    RefreshEnemyVisual();
}

void AWordQuestEnemy::RefreshEnemyVisual()
{
    if (!Visual || !EnemyWidgetComponent) return;

    if (bBoss || WaveNumber < 1 || WaveNumber > 4)
    {
        // Wave 5 remains the current boss visual until a separate boss image is chosen.
        Visual->SetVisibility(true, true);
        EnemyWidgetComponent->SetVisibility(false);
        return;
    }

    Visual->SetVisibility(false, true);
    EnemyWidgetComponent->SetVisibility(true);

    if (!EnemyWidget)
    {
        EnemyWidget = Cast<UWordQuestEnemyWidget>(EnemyWidgetComponent->GetUserWidgetObject());
    }

    if (EnemyWidget)
    {
        EnemyWidget->SetMonsterWave(WaveNumber);
    }

    // Keep each supplied monster at a natural proportion while giving all four
    // a similar on-screen height in the side-scrolling world.
    switch (WaveNumber)
    {
    case 1: // Zombie
        EnemyWidgetComponent->SetDrawSize(FVector2D(144.f, 240.f));
        EnemyWidgetComponent->SetRelativeLocation(FVector(0.f, -2.f, -100.f));
        break;

    case 2: // Skeleton archer
        EnemyWidgetComponent->SetDrawSize(FVector2D(136.f, 240.f));
        EnemyWidgetComponent->SetRelativeLocation(FVector(0.f, -2.f, -100.f));
        break;

    case 3: // Spider
        EnemyWidgetComponent->SetDrawSize(FVector2D(240.f, 188.f));
        EnemyWidgetComponent->SetRelativeLocation(FVector(0.f, -2.f, -100.f));
        break;

    case 4: // Creeper
        EnemyWidgetComponent->SetDrawSize(FVector2D(118.f, 240.f));
        EnemyWidgetComponent->SetRelativeLocation(FVector(0.f, -2.f, -100.f));
        break;

    default:
        break;
    }
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
