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
    void PressOption1();
    void PressOption2();
    void PressOption3();
    void PressOption4();
    void PressEnter();
    void PressMenuUp();
    void PressMenuDown();
    void SubmitAnswerIndex(int32 AnswerIndex);
};
