#include "WordQuestSwordPickup.h"
#include "WordQuestCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

AWordQuestSwordPickup::AWordQuestSwordPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    SetRootComponent(Trigger);
    Trigger->SetBoxExtent(FVector(80.f, 70.f, 125.f));
    Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BasicMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

    auto SetupPiece = [this, &CubeMesh, &BasicMaterial](UStaticMeshComponent* Piece)
    {
        Piece->SetupAttachment(Trigger);
        Piece->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        if (CubeMesh.Succeeded())
        {
            Piece->SetStaticMesh(CubeMesh.Object);
        }
        if (BasicMaterial.Succeeded())
        {
            Piece->SetMaterial(0, BasicMaterial.Object);
        }
    };

    // Main turquoise blade.
    Blade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Blade"));
    SetupPiece(Blade);
    Blade->SetRelativeScale3D(FVector(0.18f, 0.12f, 0.82f));
    Blade->SetRelativeLocation(FVector(0.f, 0.f, 66.f));

    // Light strip gives the blade a diamond-like shine.
    BladeHighlight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BladeHighlight"));
    SetupPiece(BladeHighlight);
    BladeHighlight->SetRelativeScale3D(FVector(0.055f, 0.125f, 0.68f));
    BladeHighlight->SetRelativeLocation(FVector(-7.f, -1.f, 68.f));

    // Dark teal cross-guard.
    Guard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Guard"));
    SetupPiece(Guard);
    Guard->SetRelativeScale3D(FVector(0.62f, 0.16f, 0.11f));
    Guard->SetRelativeLocation(FVector(0.f, 0.f, -18.f));

    // Brown handle.
    Handle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle"));
    SetupPiece(Handle);
    Handle->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.42f));
    Handle->SetRelativeLocation(FVector(0.f, 0.f, -62.f));

    // Teal pommel to finish the pixel-style diamond sword silhouette.
    Pommel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pommel"));
    SetupPiece(Pommel);
    Pommel->SetRelativeScale3D(FVector(0.30f, 0.16f, 0.10f));
    Pommel->SetRelativeLocation(FVector(0.f, 0.f, -105.f));

    // Diagonal presentation similar to the supplied diamond-sword reference.
    SetActorRotation(FRotator(0.f, 0.f, -38.f));
}

void AWordQuestSwordPickup::BeginPlay()
{
    Super::BeginPlay();

    auto ApplyColour = [](UStaticMeshComponent* Piece, const FLinearColor& Colour)
    {
        if (!Piece) return;

        if (UMaterialInstanceDynamic* MID = Piece->CreateDynamicMaterialInstance(0))
        {
            MID->SetVectorParameterValue(TEXT("Color"), Colour);
        }
    };

    // Diamond palette: bright aqua blade, pale highlight, dark teal guard/pommel.
    ApplyColour(Blade,          FLinearColor(0.18f, 0.82f, 0.78f, 1.f));
    ApplyColour(BladeHighlight, FLinearColor(0.62f, 1.00f, 0.94f, 1.f));
    ApplyColour(Guard,          FLinearColor(0.05f, 0.28f, 0.28f, 1.f));
    ApplyColour(Handle,         FLinearColor(0.48f, 0.28f, 0.13f, 1.f));
    ApplyColour(Pommel,         FLinearColor(0.05f, 0.28f, 0.28f, 1.f));
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
