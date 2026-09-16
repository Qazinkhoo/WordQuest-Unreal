#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WordQuestTypes.h"
#include "WordQuestQuestionSubsystem.generated.h"

UCLASS()
class WORDQUEST_API UWordQuestQuestionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category="Word Quest|Questions") void ResetAdventureQuestions();
    UFUNCTION(BlueprintCallable, Category="Word Quest|Questions") bool GetNextQuestion(FWordQuestQuestion& OutQuestion);

private:
    TArray<FWordQuestQuestion> QuestionBank;
    TArray<int32> RemainingQuestionIndices;
    void BuildYear4QuestionBank();
};
