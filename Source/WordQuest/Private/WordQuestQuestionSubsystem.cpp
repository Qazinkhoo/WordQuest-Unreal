#include "WordQuestQuestionSubsystem.h"
#include "Algo/RandomShuffle.h"

void UWordQuestQuestionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    BuildYear4QuestionBank();
    ResetAdventureQuestions();
}

void UWordQuestQuestionSubsystem::ResetAdventureQuestions()
{
    RemainingQuestionIndices.Empty();
    for (int32 i = 0; i < QuestionBank.Num(); ++i) RemainingQuestionIndices.Add(i);
    Algo::RandomShuffle(RemainingQuestionIndices);
}

bool UWordQuestQuestionSubsystem::GetNextQuestion(FWordQuestQuestion& OutQuestion)
{
    if (RemainingQuestionIndices.IsEmpty()) return false;

    const int32 Index = RemainingQuestionIndices.Pop();
    OutQuestion = QuestionBank[Index];

    // Shuffle answer positions while preserving the correct-answer index.
    for (int32 i = OutQuestion.Answers.Num() - 1; i > 0; --i)
    {
        const int32 j = FMath::RandRange(0, i);
        if (i == j) continue;

        OutQuestion.Answers.Swap(i, j);
        if (OutQuestion.CorrectAnswerIndex == i) OutQuestion.CorrectAnswerIndex = j;
        else if (OutQuestion.CorrectAnswerIndex == j) OutQuestion.CorrectAnswerIndex = i;
    }

    return true;
}

void UWordQuestQuestionSubsystem::BuildYear4QuestionBank()
{
    QuestionBank.Empty();
    auto Add = [this](const TCHAR* Id, const TCHAR* Prompt, std::initializer_list<const TCHAR*> Options, int32 Correct, const TCHAR* Category)
    {
        FWordQuestQuestion Q;
        Q.Id = FName(Id);
        Q.Prompt = Prompt;
        Q.CorrectAnswerIndex = Correct;
        Q.Category = Category;
        for (const TCHAR* Option : Options) Q.Answers.Add(Option);
        QuestionBank.Add(Q);
    };

    Add(TEXT("Y4_001"), TEXT("Aina ___ to school every morning."), {TEXT("walk"), TEXT("walks"), TEXT("walking"), TEXT("walked")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_002"), TEXT("Yesterday, we ___ football after school."), {TEXT("play"), TEXT("plays"), TEXT("played"), TEXT("playing")}, 2, TEXT("Tenses"));
    Add(TEXT("Y4_003"), TEXT("The cat is hiding ___ the table."), {TEXT("under"), TEXT("during"), TEXT("because"), TEXT("quickly")}, 0, TEXT("Prepositions"));
    Add(TEXT("Y4_004"), TEXT("Which word means the opposite of 'noisy'?"), {TEXT("loud"), TEXT("quiet"), TEXT("busy"), TEXT("bright")}, 1, TEXT("Vocabulary"));
    Add(TEXT("Y4_005"), TEXT("Choose the correct spelling."), {TEXT("becaus"), TEXT("becouse"), TEXT("because"), TEXT("beacause")}, 2, TEXT("Spelling"));
    Add(TEXT("Y4_006"), TEXT("Sara is ___ than her younger sister."), {TEXT("tall"), TEXT("taller"), TEXT("tallest"), TEXT("more tall")}, 1, TEXT("Comparatives"));
    Add(TEXT("Y4_007"), TEXT("I was hungry, ___ I ate a sandwich."), {TEXT("so"), TEXT("but"), TEXT("or"), TEXT("until")}, 0, TEXT("Conjunctions"));
    Add(TEXT("Y4_008"), TEXT("Which sentence is punctuated correctly?"), {TEXT("Where are you going."), TEXT("Where are you going?"), TEXT("where are you going?"), TEXT("Where are you going!")}, 1, TEXT("Punctuation"));
    Add(TEXT("Y4_009"), TEXT("The pupils ___ reading a story now."), {TEXT("is"), TEXT("are"), TEXT("was"), TEXT("be")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_010"), TEXT("Which word is an adjective?"), {TEXT("beautiful"), TEXT("quickly"), TEXT("jump"), TEXT("school")}, 0, TEXT("Word Classes"));
    Add(TEXT("Y4_011"), TEXT("My father ___ dinner last night."), {TEXT("cook"), TEXT("cooks"), TEXT("cooked"), TEXT("cooking")}, 2, TEXT("Tenses"));
    Add(TEXT("Y4_012"), TEXT("We use an umbrella when it is ___."), {TEXT("raining"), TEXT("sleeping"), TEXT("reading"), TEXT("singing")}, 0, TEXT("Vocabulary"));
    Add(TEXT("Y4_013"), TEXT("Which word is a conjunction?"), {TEXT("and"), TEXT("slowly"), TEXT("green"), TEXT("teacher")}, 0, TEXT("Word Classes"));
    Add(TEXT("Y4_014"), TEXT("There ___ three books on the desk."), {TEXT("is"), TEXT("are"), TEXT("was"), TEXT("has")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_015"), TEXT("Amir can swim, ___ he cannot dive."), {TEXT("but"), TEXT("because"), TEXT("so"), TEXT("and then")}, 0, TEXT("Conjunctions"));
    Add(TEXT("Y4_016"), TEXT("Which word means almost the same as 'happy'?"), {TEXT("glad"), TEXT("angry"), TEXT("tired"), TEXT("afraid")}, 0, TEXT("Vocabulary"));
    Add(TEXT("Y4_017"), TEXT("The rabbit ran ___."), {TEXT("quick"), TEXT("quickly"), TEXT("quicker"), TEXT("quickness")}, 1, TEXT("Adverbs"));
    Add(TEXT("Y4_018"), TEXT("Tomorrow, I ___ visit my grandmother."), {TEXT("will"), TEXT("did"), TEXT("was"), TEXT("has")}, 0, TEXT("Future Tense"));
    Add(TEXT("Y4_019"), TEXT("Choose the correct plural of 'child'."), {TEXT("childs"), TEXT("childes"), TEXT("children"), TEXT("childrens")}, 2, TEXT("Grammar"));
    Add(TEXT("Y4_020"), TEXT("Which sentence is a question?"), {TEXT("Please close the door."), TEXT("The door is closed."), TEXT("Did you close the door?"), TEXT("Close the door!")}, 2, TEXT("Sentence Types"));
    Add(TEXT("Y4_021"), TEXT("Mira ___ her teeth before bed every night."), {TEXT("brush"), TEXT("brushes"), TEXT("brushed"), TEXT("brushing")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_022"), TEXT("We ___ at the museum last Saturday."), {TEXT("are"), TEXT("were"), TEXT("is"), TEXT("be")}, 1, TEXT("Tenses"));
    Add(TEXT("Y4_023"), TEXT("The ball rolled ___ the chair."), {TEXT("behind"), TEXT("because"), TEXT("carefully"), TEXT("tomorrow")}, 0, TEXT("Prepositions"));
    Add(TEXT("Y4_024"), TEXT("Which word means the opposite of 'empty'?"), {TEXT("full"), TEXT("quiet"), TEXT("weak"), TEXT("thin")}, 0, TEXT("Vocabulary"));
    Add(TEXT("Y4_025"), TEXT("Choose the correct spelling."), {TEXT("freind"), TEXT("friend"), TEXT("frend"), TEXT("friand")}, 1, TEXT("Spelling"));
    Add(TEXT("Y4_026"), TEXT("This box is ___ than that one."), {TEXT("heavy"), TEXT("heavier"), TEXT("heaviest"), TEXT("more heavy")}, 1, TEXT("Comparatives"));
    Add(TEXT("Y4_027"), TEXT("Nina was tired, ___ she finished her homework."), {TEXT("but"), TEXT("so"), TEXT("because"), TEXT("or")}, 0, TEXT("Conjunctions"));
    Add(TEXT("Y4_028"), TEXT("Which sentence uses a capital letter correctly?"), {TEXT("we live in Malaysia."), TEXT("We live in malaysia."), TEXT("We live in Malaysia."), TEXT("we live in malaysia.")}, 2, TEXT("Punctuation"));
    Add(TEXT("Y4_029"), TEXT("The boys ___ playing badminton now."), {TEXT("is"), TEXT("are"), TEXT("was"), TEXT("has")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_030"), TEXT("Which word is an adverb?"), {TEXT("slowly"), TEXT("slow"), TEXT("runner"), TEXT("road")}, 0, TEXT("Word Classes"));
    Add(TEXT("Y4_031"), TEXT("She ___ a letter yesterday."), {TEXT("write"), TEXT("writes"), TEXT("wrote"), TEXT("writing")}, 2, TEXT("Tenses"));
    Add(TEXT("Y4_032"), TEXT("A doctor usually works in a ___."), {TEXT("hospital"), TEXT("library"), TEXT("bakery"), TEXT("stadium")}, 0, TEXT("Vocabulary"));
    Add(TEXT("Y4_033"), TEXT("Which word is a noun?"), {TEXT("carefully"), TEXT("yellow"), TEXT("teacher"), TEXT("jumped")}, 2, TEXT("Word Classes"));
    Add(TEXT("Y4_034"), TEXT("There ___ an apple in the bag."), {TEXT("are"), TEXT("is"), TEXT("were"), TEXT("have")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_035"), TEXT("I stayed indoors ___ it was raining."), {TEXT("because"), TEXT("but"), TEXT("or"), TEXT("so that")}, 0, TEXT("Conjunctions"));
    Add(TEXT("Y4_036"), TEXT("Which word means almost the same as 'small'?"), {TEXT("tiny"), TEXT("huge"), TEXT("noisy"), TEXT("fast")}, 0, TEXT("Vocabulary"));
    Add(TEXT("Y4_037"), TEXT("The baby slept ___."), {TEXT("peaceful"), TEXT("peacefully"), TEXT("peace"), TEXT("more peaceful")}, 1, TEXT("Adverbs"));
    Add(TEXT("Y4_038"), TEXT("Next week, we ___ visit the science centre."), {TEXT("will"), TEXT("were"), TEXT("did"), TEXT("has")}, 0, TEXT("Future Tense"));
    Add(TEXT("Y4_039"), TEXT("Choose the correct plural of 'tooth'."), {TEXT("tooths"), TEXT("teeth"), TEXT("toothes"), TEXT("teeths")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_040"), TEXT("Which sentence is an instruction?"), {TEXT("Please open your book."), TEXT("My book is blue."), TEXT("Where is my book?"), TEXT("What a lovely book!")}, 0, TEXT("Sentence Types"));
    Add(TEXT("Y4_041"), TEXT("Farah ___ breakfast at 7 o'clock every day."), {TEXT("eat"), TEXT("eats"), TEXT("ate"), TEXT("eating")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_042"), TEXT("Last night, the baby ___ for eight hours."), {TEXT("sleep"), TEXT("sleeps"), TEXT("slept"), TEXT("sleeping")}, 2, TEXT("Tenses"));
    Add(TEXT("Y4_043"), TEXT("The picture is hanging ___ the wall."), {TEXT("on"), TEXT("during"), TEXT("carefully"), TEXT("because")}, 0, TEXT("Prepositions"));
    Add(TEXT("Y4_044"), TEXT("Which word means the opposite of 'strong'?"), {TEXT("weak"), TEXT("brave"), TEXT("hard"), TEXT("wide")}, 0, TEXT("Vocabulary"));
    Add(TEXT("Y4_045"), TEXT("Choose the correct spelling."), {TEXT("beautiful"), TEXT("beautifull"), TEXT("butiful"), TEXT("beutiful")}, 0, TEXT("Spelling"));
    Add(TEXT("Y4_046"), TEXT("A cheetah is ___ than a tortoise."), {TEXT("fast"), TEXT("faster"), TEXT("fastest"), TEXT("more fast")}, 1, TEXT("Comparatives"));
    Add(TEXT("Y4_047"), TEXT("You can have tea ___ juice."), {TEXT("or"), TEXT("because"), TEXT("so"), TEXT("although")}, 0, TEXT("Conjunctions"));
    Add(TEXT("Y4_048"), TEXT("Which sentence ends with the correct punctuation?"), {TEXT("What a surprise?"), TEXT("What a surprise!"), TEXT("What a surprise,"), TEXT("What a surprise")}, 1, TEXT("Punctuation"));
    Add(TEXT("Y4_049"), TEXT("My sister ___ a new bicycle."), {TEXT("have"), TEXT("has"), TEXT("having"), TEXT("had been")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_050"), TEXT("Which word is an adjective?"), {TEXT("bright"), TEXT("brightly"), TEXT("shine"), TEXT("lamp")}, 0, TEXT("Word Classes"));
}
