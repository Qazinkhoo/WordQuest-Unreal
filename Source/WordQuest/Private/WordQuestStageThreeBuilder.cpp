#include "WordQuestStageThreeBuilder.h"
#include "WordQuestEnemy.h"
#include "WordQuestCharacter.h"
#include "WordQuestStageBackground.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

AWordQuestStageThreeBuilder::AWordQuestStageThreeBuilder()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWordQuestStageThreeBuilder::SpawnBlock(const FVector& Location, const FVector& Scale, const FRotator& Rotation)
{
    if (!GetWorld()) return;
    AStaticMeshActor* Block = GetWorld()->SpawnActor<AStaticMeshActor>(Location, Rotation);
    if (!Block) return;

    static UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (!CubeMesh) return;

    Block->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
    Block->SetActorScale3D(Scale);
    Block->GetStaticMeshComponent()->SetMobility(EComponentMobility::Static);
}

void AWordQuestStageThreeBuilder::SpawnCrystalCluster(const FVector& Location, float Scale)
{
    SpawnBlock(Location + FVector(0.f, 0.f, 80.f * Scale), FVector(0.18f, 0.28f, 0.85f) * Scale, FRotator(0.f, 0.f, 18.f));
    SpawnBlock(Location + FVector(42.f * Scale, 0.f, 62.f * Scale), FVector(0.15f, 0.22f, 0.62f) * Scale, FRotator(0.f, 0.f, -24.f));
    SpawnBlock(Location + FVector(-38.f * Scale, 0.f, 55.f * Scale), FVector(0.13f, 0.20f, 0.52f) * Scale, FRotator(0.f, 0.f, 30.f));
}

void AWordQuestStageThreeBuilder::SpawnCaveRib(const FVector& Location, float Scale)
{
    SpawnBlock(Location + FVector(-105.f * Scale, 0.f, 150.f * Scale), FVector(0.26f, 0.75f, 1.45f) * Scale);
    SpawnBlock(Location + FVector(105.f * Scale, 0.f, 150.f * Scale), FVector(0.26f, 0.75f, 1.45f) * Scale);
    SpawnBlock(Location + FVector(0.f, 0.f, 292.f * Scale), FVector(1.28f, 0.75f, 0.24f) * Scale);
}

void AWordQuestStageThreeBuilder::BeginPlay()
{
    Super::BeginPlay();
    const FVector Origin = GetActorLocation();

    if (UWorld* World = GetWorld())
    {
        AWordQuestStageBackground* Background = World->SpawnActor<AWordQuestStageBackground>(Origin, FRotator::ZeroRotator);
        if (Background)
        {
            Background->ConfigureBackground(3, Origin);
        }
    }

    for (int32 i = 0; i < 19; ++i)
    {
        SpawnBlock(Origin + FVector(200.f + i * 400.f, 0.f, -50.f), FVector(4.f, 6.f, 1.f));
    }

    for (int32 i = 0; i < 16; ++i)
    {
        const float X = 360.f + i * 390.f;
        SpawnCrystalCluster(Origin + FVector(X, 300.f, 0.f), 0.85f + (i % 3) * 0.12f);
        SpawnCrystalCluster(Origin + FVector(X + 170.f, -300.f, 0.f), 0.72f + (i % 2) * 0.14f);
    }

    SpawnCaveRib(Origin + FVector(900.f, 330.f, 0.f), 0.95f);
    SpawnCaveRib(Origin + FVector(3150.f, -330.f, 0.f), 0.90f);
    SpawnCaveRib(Origin + FVector(5450.f, 330.f, 0.f), 1.00f);

    const float CeilingX[] = {650.f, 1450.f, 2250.f, 3000.f, 3900.f, 4750.f, 5650.f, 6500.f};
    for (int32 i = 0; i < 8; ++i)
    {
        SpawnBlock(Origin + FVector(CeilingX[i], 0.f, 520.f), FVector(0.22f + (i % 3) * 0.05f, 0.55f, 0.85f), FRotator(0.f, 0.f, (i % 2 == 0) ? 8.f : -10.f));
    }

    const float EnemyX[] = {1300.f, 2400.f, 3500.f, 4650.f, 5900.f};
    for (int32 Wave = 1; Wave <= 5; ++Wave)
    {
        AWordQuestEnemy* Enemy = GetWorld()->SpawnActor<AWordQuestEnemy>(Origin + FVector(EnemyX[Wave - 1], 0.f, 100.f), FRotator::ZeroRotator);
        if (Enemy)
        {
            Enemy->ConfigureEnemy(Wave, Wave == 5);
        }
    }

    if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        Player->SetActorLocation(Origin + FVector(100.f, 0.f, 120.f), false, nullptr, ETeleportType::TeleportPhysics);
        Player->SetBattleLocked(false);
    }
}
