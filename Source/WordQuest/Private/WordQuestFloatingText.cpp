#include "WordQuestFloatingText.h"
#include "Components/TextRenderComponent.h"

AWordQuestFloatingText::AWordQuestFloatingText()
{
    PrimaryActorTick.bCanEverTick = true;

    TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextComponent"));
    SetRootComponent(TextComponent);
    TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    TextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
    TextComponent->SetWorldSize(42.0f);
    TextComponent->SetTextRenderColor(FColor::White);
    SetActorRotation(FRotator(0.f, 90.f, 0.f));
}

void AWordQuestFloatingText::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AddActorWorldOffset(FVector(0.f, 0.f, MoveSpeed * DeltaTime));
    LifeTime -= DeltaTime;
    if (LifeTime <= 0.f)
    {
        Destroy();
    }
}

void AWordQuestFloatingText::SetupText(const FString& InText, const FColor& InColor)
{
    if (!TextComponent) return;
    TextComponent->SetText(FText::FromString(InText));
    TextComponent->SetTextRenderColor(InColor);
}
