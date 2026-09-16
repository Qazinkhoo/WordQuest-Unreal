#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WordQuestPlayerController.generated.h"

UCLASS()
class WORDQUEST_API AWordQuestPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void SetupInputComponent() override;

private:
    void SubmitAnswer1();
    void SubmitAnswer2();
    void SubmitAnswer3();
    void SubmitAnswer4();
    void SubmitAnswerIndex(int32 AnswerIndex);
};
