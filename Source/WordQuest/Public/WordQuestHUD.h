#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WordQuestHUD.generated.h"

struct FWordQuestHUDMessage
{
    FString Text;
    FColor Color = FColor::White;
    FVector WorldLocation = FVector::ZeroVector;
    float StartTime = 0.f;
    float Duration = 1.35f;
};

UCLASS()
class WORDQUEST_API AWordQuestHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

    void AddFloatingMessage(const FString& InText, const FColor& InColor, const FVector& InWorldLocation, float InDuration = 1.35f);

private:
    TArray<FWordQuestHUDMessage> FloatingMessages;
    void DrawFloatingMessages();
};
