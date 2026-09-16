#include "WordQuestStageOneBuilder.h"
#include "WordQuestEnemy.h"
#include "WordQuestSwordPickup.h"
#include "WordQuestStageBackground.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AWordQuestStageOneBuilder::AWordQuestStageOneBuilder()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWordQuestStageOneBuilder::SpawnBlock(const FVector& Location, const FVector& Scale)
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

void AWordQuestStageOneBuilder::SpawnTree(const FVector& Location, float Scale)
{
    SpawnBlock(Location + FVector(0.f, 0.f, 150.f * Scale), FVector(0.45f, 0.45f, 1.5f) * Scale);
    SpawnBlock(Location + FVector(0.f, 0.f, 330.f * Scale), FVector(1.3f, 1.3f, 0.7f) * Scale);
    SpawnBlock(Location + FVector(70.f * Scale, 0.f, 400.f * Scale), FVector(0.8f, 0.8f, 0.6f) * Scale);
    SpawnBlock(Location + FVector(-70.f * Scale, 0.f, 400.f * Scale), FVector(0.8f, 0.8f, 0.6f) * Scale);
}

void AWordQuestStageOneBuilder::BeginPlay()
{
    Super::BeginPlay();

    const FVector Origin = GetActorLocation();

    if (UWorld* World = GetWorld())
    {
        AWordQuestStageBackground* Background = World->SpawnActor<AWordQuestStageBackground>(Origin, FRotator::ZeroRotator);
        if (Background)
        {
            Background->ConfigureBackground(1, Origin);
            Background->SetActorLabel(TEXT("Stage1_Background"));
        }
    }

    // Build a continuous collision floor for the whole stage instead of relying
    // on the small Basic-level editor floor. Each cube is 400 units long, and
    // together they cover the route past the Wave 5 boss.
    for (int32 i = 0; i < 19; ++i)
    {
        SpawnBlock(Origin + FVector(200.f + i * 400.f, 0.f, -50.f), FVector(4.f, 6.f, 1.f));
    }

    const float TreeX[] = { 350.f, 900.f, 1500.f, 2150.f, 2850.f, 3550.f, 4300.f, 5050.f, 5900.f };
    for (int32 i = 0; i < 9; ++i)
    {
        SpawnTree(Origin + FVector(TreeX[i], 220.f, 0.f), 0.85f + (i % 3) * 0.12f);
        SpawnTree(Origin + FVector(TreeX[i] + 260.f, -260.f, 0.f), 0.75f + (i % 2) * 0.15f);
    }

    for (int32 i = 0; i < 16; ++i)
    {
        SpawnBlock(Origin + FVector(250.f + i * 380.f, 360.f, 35.f), FVector(1.4f, 0.5f, 0.35f));
    }

    if (UWorld* World = GetWorld())
    {
        AWordQuestSwordPickup* Sword = World->SpawnActor<AWordQuestSwordPickup>(Origin + FVector(650.f, 0.f, 100.f), FRotator::ZeroRotator);
        if (Sword) Sword->SetActorLabel(TEXT("Stage1_Sword"));

        const float EnemyX[] = { 1500.f, 2600.f, 3700.f, 4800.f, 6000.f };
        for (int32 Wave = 1; Wave <= 5; ++Wave)
        {
            AWordQuestEnemy* Enemy = World->SpawnActor<AWordQuestEnemy>(Origin + FVector(EnemyX[Wave - 1], 0.f, 100.f), FRotator::ZeroRotator);
            if (Enemy)
            {
                Enemy->ConfigureEnemy(Wave, Wave == 5);
                Enemy->SetActorLabel(FString::Printf(TEXT("Stage1_Enemy_Wave_%d"), Wave));
            }
        }
    }
}
