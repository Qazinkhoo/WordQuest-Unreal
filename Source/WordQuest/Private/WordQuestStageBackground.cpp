#include "WordQuestStageBackground.h"
#include "WordQuestStageBackgroundWidget.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"

AWordQuestStageBackground::AWordQuestStageBackground()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    BackgroundWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("BackgroundWidgetComponent"));
    BackgroundWidgetComponent->SetupAttachment(Root);
    BackgroundWidgetComponent->SetWidgetClass(UWordQuestStageBackgroundWidget::StaticClass());
    BackgroundWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

    // A wide canvas is filled with many correctly proportioned copies of the
    // supplied stage image. This avoids the extreme horizontal stretching that
    // happened when a single portrait image filled a 16:9 panel.
    BackgroundWidgetComponent->SetDrawSize(FVector2D(7680.f, 1440.f));
    BackgroundWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
    BackgroundWidgetComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    BackgroundWidgetComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
    BackgroundWidgetComponent->SetTwoSided(true);
    BackgroundWidgetComponent->SetBlendMode(EWidgetBlendMode::Opaque);
    BackgroundWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Cap both ends of the designed route. These components are invisible in
    // game but retain collision, so there is no longer an accessible "infinite"
    // empty area before or after the playable stage.
    LeftBoundary = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftStageBoundary"));
    LeftBoundary->SetupAttachment(Root);
    LeftBoundary->SetBoxExtent(FVector(60.f, 650.f, 900.f));
    LeftBoundary->SetCollisionProfileName(TEXT("BlockAll"));
    LeftBoundary->SetGenerateOverlapEvents(false);
    LeftBoundary->SetVisibility(false);
    LeftBoundary->SetHiddenInGame(true);

    RightBoundary = CreateDefaultSubobject<UBoxComponent>(TEXT("RightStageBoundary"));
    RightBoundary->SetupAttachment(Root);
    RightBoundary->SetBoxExtent(FVector(60.f, 650.f, 900.f));
    RightBoundary->SetCollisionProfileName(TEXT("BlockAll"));
    RightBoundary->SetGenerateOverlapEvents(false);
    RightBoundary->SetVisibility(false);
    RightBoundary->SetHiddenInGame(true);
}

void AWordQuestStageBackground::BeginPlay()
{
    Super::BeginPlay();

    if (BackgroundWidgetComponent)
    {
        BackgroundWidget = Cast<UWordQuestStageBackgroundWidget>(BackgroundWidgetComponent->GetUserWidgetObject());
    }

    RefreshBackground();
}

void AWordQuestStageBackground::ConfigureBackground(int32 InStageNumber, const FVector& StageOrigin)
{
    StageNumber = FMath::Clamp(InStageNumber, 1, 3);

    // Keep the repeated backdrop behind gameplay and centred on the complete
    // playable route. The wider panel extends beyond both stage ends so the
    // camera never exposes the default empty world beside the artwork.
    SetActorLocation(StageOrigin + FVector(3400.f, -900.f, 650.f));

    // The generated gameplay route is about 7,600 Unreal units long. Keep the
    // player inside those limits while leaving enough room around the final boss.
    if (LeftBoundary)
    {
        LeftBoundary->SetWorldLocation(StageOrigin + FVector(-220.f, 0.f, 600.f));
    }
    if (RightBoundary)
    {
        RightBoundary->SetWorldLocation(StageOrigin + FVector(7700.f, 0.f, 600.f));
    }

    RefreshBackground();
}

void AWordQuestStageBackground::RefreshBackground()
{
    if (!BackgroundWidget && BackgroundWidgetComponent)
    {
        BackgroundWidget = Cast<UWordQuestStageBackgroundWidget>(BackgroundWidgetComponent->GetUserWidgetObject());
    }

    if (BackgroundWidget)
    {
        BackgroundWidget->SetStageNumber(StageNumber);
    }
}
