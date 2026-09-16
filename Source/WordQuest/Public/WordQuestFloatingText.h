#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WordQuestFloatingText.generated.h"

class UTextRenderComponent;

UCLASS()
class WORDQUEST_API AWordQuestFloatingText : public AActor
{
    GENERATED_BODY()

public:
    AWordQuestFloatingText();
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    void SetupText(const FString& InText, const FColor& InColor);

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UTextRenderComponent> TextComponent;

    float LifeTime = 1.0f;
    float MoveSpeed = 80.0f;
};
