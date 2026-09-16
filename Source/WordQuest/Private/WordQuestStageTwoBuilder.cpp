#include "WordQuestStageTwoBuilder.h"
#include "WordQuestEnemy.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"

AWordQuestStageTwoBuilder::AWordQuestStageTwoBuilder()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWordQuestStageTwoBuilder::SpawnBlock(const FVector& Location, const FVector& Scale)
{
    UWorld* World = GetWorld();
    if (!World) return;

    AStaticMeshActor* Block = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator);
    if (!Block) return;

    static UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh)
    {
        Block->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
        Block->SetActorScale3D(Scale);
        Block->GetStaticMeshComponent()->SetMobility(EComponentMobility::Static);
    }
}

void AWordQuestStageTwoBuilder::SpawnFlower(const FVector& Location, float Scale)
{
    SpawnBlock(Location + FVector(0.f, 0.f, 45.f * Scale), FVector(0.10f, 0.10f, 0.45f) * Scale);
    SpawnBlock(Location + FVector(0.f, 0.f, 95.f * Scale), FVector(0.28f, 0.28f, 0.18f) * Scale);
}

void AWordQuestStageTwoBuilder::SpawnMeadowArch(const FVector& Location, float Scale)
{
    SpawnBlock(Location + FVector(-90.f * Scale, 0.f, 115.f * Scale), FVector(0.25f, 0.8f, 1.15f) * Scale);
    SpawnBlock(Location + FVector(90.f * Scale, 0.f, 115.f * Scale), FVector(0.25f, 0.8f, 1.15f) * Scale);
    SpawnBlock(Location + FVector(0.f, 0.f, 230.f * Scale), FVector(1.15f, 0.8f, 0.25f) * Scale);
}

void AWordQuestStageTwoBuilder::BeginPlay()
{
    Super::BeginPlay();

    const FVector Origin = GetActorLocation();

    // Long continuous meadow floor beyond the Stage 1 area.
    for (int32 i = 0; i < 19; ++i)
    {
        SpawnBlock(Origin + FVector(200.f + i * 400.f, 0.f, -50.f), FVector(4.f, 6.f, 1.f));
    }

    // Meadow markers and flowers arranged so they do not obstruct the character path.
    for (int32 i = 0; i < 18; ++i)
    {
        const float X = 300.f + i * 360.f;
        SpawnFlower(Origin + FVector(X, 260.f, 0.f), 0.9f + (i % 3) * 0.10f);
        SpawnFlower(Origin + FVector(X + 140.f, -280.f, 0.f), 0.8f + (i % 2) * 0.12f);
    }

    SpawnMeadowArch(Origin + FVector(850.f, 330.f, 0.f), 0.9f);
    SpawnMeadowArch(Origin + FVector(3100.f, -330.f, 0.f), 0.85f);
    SpawnMeadowArch(Origin + FVector(5350.f, 330.f, 0.f), 0.95f);

    if (UWorld* World = GetWorld())
    {
        const float EnemyX[] = { 1300.f, 2400.f, 3500.f, 4650.f, 5900.f };
        for (int32 Wave = 1; Wave <= 5; ++Wave)
        {
            AWordQuestEnemy* Enemy = World->SpawnActor<AWordQuestEnemy>(Origin + FVector(EnemyX[Wave - 1], 0.f, 100.f), FRotator::ZeroRotator);
            if (Enemy)
            {
                Enemy->ConfigureEnemy(Wave, Wave == 5);
                Enemy->SetActorLabel(FString::Printf(TEXT("Stage2_Enemy_Wave_%d"), Wave));
            }
        }
    }
}
