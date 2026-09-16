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
    BackgroundWidgetComponent->SetDrawSize(FVector2D(1920.f, 1080.f));
    BackgroundWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
    BackgroundWidgetComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    BackgroundWidgetComponent->SetRelativeScale3D(FVector(4.4f, 4.4f, 4.4f));
    BackgroundWidgetComponent->SetTwoSided(true);
    BackgroundWidgetComponent->SetBlendMode(EWidgetBlendMode::Opaque);
    BackgroundWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BackgroundWidgetComponent->SetTranslucencySortPriority(-100);
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

    // The side camera sits on +Y and looks toward -Y, so place this giant image
    // behind all gameplay actors. One 1920x1080 widget scaled to 4.4 spans the
    // complete playable route while keeping the foreground blocks interactive.
    SetActorLocation(StageOrigin + FVector(3400.f, -850.f, 1150.f));

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
