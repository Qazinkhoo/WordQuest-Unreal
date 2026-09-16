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

    // Sprite size is controlled by the widget component, so keep the actor itself
    // at a stable scale for consistent collision and encounter distance.
    SetActorScale3D(FVector(1.f));

    RefreshEnemyVisual();
}

void AWordQuestEnemy::RefreshEnemyVisual()
{
    if (!Visual || !EnemyWidgetComponent) return;

    // All normal monsters and all three bosses now use supplied sprite artwork.
    Visual->SetVisibility(false, true);
    EnemyWidgetComponent->SetVisibility(true);

    if (!EnemyWidget)
    {
        EnemyWidget = Cast<UWordQuestEnemyWidget>(EnemyWidgetComponent->GetUserWidgetObject());
    }

    int32 StageNumber = 1;
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>())
    {
        StageNumber = FMath::Clamp(GI->PlayerState.Stage, 1, 3);
    }

    if (EnemyWidget)
    {
        EnemyWidget->SetEnemyVisual(WaveNumber, StageNumber, bBoss);
    }

    if (bBoss)
    {
        switch (StageNumber)
        {
        case 1: // Ghast - floating cube boss
            EnemyWidgetComponent->SetDrawSize(FVector2D(220.f, 252.f));
            EnemyWidgetComponent->SetRelativeLocation(FVector(0.f, -2.f, -35.f));
            break;

        case 2: // Enderman - tall boss with particles
            EnemyWidgetComponent->SetDrawSize(FVector2D(230.f, 255.f));
            EnemyWidgetComponent->SetRelativeLocation(FVector(0.f, -2.f, -100.f));
            break;

        case 3: // Ender Dragon - wide final boss
        default:
            EnemyWidgetComponent->SetDrawSize(FVector2D(360.f, 220.f));
            EnemyWidgetComponent->SetRelativeLocation(FVector(0.f, -2.f, -45.f));
            break;
        }

        return;
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

    const FVector BaseScale = GetActorScale3D();
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
