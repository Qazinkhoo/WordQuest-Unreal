#include "WordQuestStageBackground.h"
#include "WordQuestStageBackgroundWidget.h"
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
    // playable route. The wider panel now extends beyond both stage ends so the
    // camera never exposes the default empty world beside the artwork.
    SetActorLocation(StageOrigin + FVector(3400.f, -900.f, 650.f));

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
