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
    for (int32 i = 0; i < QuestionBank.Num(); ++i)
    {
        RemainingQuestionIndices.Add(i);
    }
    Algo::RandomShuffle(RemainingQuestionIndices);
}

void UWordQuestQuestionSubsystem::ShuffleAnswers(FWordQuestQuestion& Question) const
{
    for (int32 i = Question.Answers.Num() - 1; i > 0; --i)
    {
        const int32 j = FMath::RandRange(0, i);
        if (i == j) continue;

        Question.Answers.Swap(i, j);
        if (Question.CorrectAnswerIndex == i) Question.CorrectAnswerIndex = j;
        else if (Question.CorrectAnswerIndex == j) Question.CorrectAnswerIndex = i;
    }
}

int32 UWordQuestQuestionSubsystem::GetDifficultyScore(const FWordQuestQuestion& Question) const
{
    const FString& Category = Question.Category;
    if (Category == TEXT("Vocabulary") || Category == TEXT("Spelling") || Category == TEXT("Prepositions")) return 1;
    if (Category == TEXT("Grammar") || Category == TEXT("Word Classes")) return 2;
    if (Category == TEXT("Tenses") || Category == TEXT("Future Tense") || Category == TEXT("Comparatives") || Category == TEXT("Adverbs")) return 3;
    if (Category == TEXT("Conjunctions")) return 4;
    if (Category == TEXT("Punctuation") || Category == TEXT("Sentence Types")) return 5;
    return 3;
}

bool UWordQuestQuestionSubsystem::GetNextQuestion(FWordQuestQuestion& OutQuestion)
{
    if (RemainingQuestionIndices.IsEmpty()) return false;

    const int32 Position = FMath::RandRange(0, RemainingQuestionIndices.Num() - 1);
    const int32 QuestionIndex = RemainingQuestionIndices[Position];
    RemainingQuestionIndices.RemoveAtSwap(Position);

    OutQuestion = QuestionBank[QuestionIndex];
    ShuffleAnswers(OutQuestion);
    return true;
}

bool UWordQuestQuestionSubsystem::GetNextQuestionForWave(int32 Wave, FWordQuestQuestion& OutQuestion)
{
    if (RemainingQuestionIndices.IsEmpty()) return false;

    const int32 TargetDifficulty = FMath::Clamp(Wave, 1, 5);
    TArray<int32> CandidatePositions;

    for (int32 Distance = 0; Distance <= 4; ++Distance)
    {
        CandidatePositions.Reset();
        for (int32 Position = 0; Position < RemainingQuestionIndices.Num(); ++Position)
        {
            const int32 QuestionIndex = RemainingQuestionIndices[Position];
            if (!QuestionBank.IsValidIndex(QuestionIndex)) continue;

            if (FMath::Abs(GetDifficultyScore(QuestionBank[QuestionIndex]) - TargetDifficulty) == Distance)
            {
                CandidatePositions.Add(Position);
            }
        }
        if (!CandidatePositions.IsEmpty()) break;
    }

    if (CandidatePositions.IsEmpty()) return GetNextQuestion(OutQuestion);

    const int32 Position = CandidatePositions[FMath::RandRange(0, CandidatePositions.Num() - 1)];
    const int32 QuestionIndex = RemainingQuestionIndices[Position];
    RemainingQuestionIndices.RemoveAtSwap(Position);

    OutQuestion = QuestionBank[QuestionIndex];
    ShuffleAnswers(OutQuestion);
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

    // Do not call this lambda AddDynamic: Unreal defines AddDynamic as a delegate macro.
    auto AddGeneratedQuestion = [this](const FString& Id, const FString& Prompt, const TArray<FString>& Options, int32 Correct, const FString& Category)
    {
        FWordQuestQuestion Q;
        Q.Id = FName(*Id);
        Q.Prompt = Prompt;
        Q.Answers = Options;
        Q.CorrectAnswerIndex = Correct;
        Q.Category = Category;
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

    const TCHAR* OppositeWords[][2] = {
        {TEXT("hot"), TEXT("cold")}, {TEXT("big"), TEXT("small")}, {TEXT("fast"), TEXT("slow")}, {TEXT("old"), TEXT("young")},
        {TEXT("clean"), TEXT("dirty")}, {TEXT("happy"), TEXT("sad")}, {TEXT("light"), TEXT("dark")}, {TEXT("open"), TEXT("closed")},
        {TEXT("early"), TEXT("late")}, {TEXT("soft"), TEXT("hard")}, {TEXT("high"), TEXT("low")}, {TEXT("wet"), TEXT("dry")},
        {TEXT("inside"), TEXT("outside")}, {TEXT("near"), TEXT("far")}, {TEXT("full"), TEXT("empty")}, {TEXT("strong"), TEXT("weak")},
        {TEXT("thick"), TEXT("thin")}, {TEXT("noisy"), TEXT("quiet")}, {TEXT("safe"), TEXT("dangerous")}, {TEXT("kind"), TEXT("unkind")}
    };

    for (int32 i = 0; i < 20; ++i)
    {
        AddGeneratedQuestion(FString::Printf(TEXT("EASY_OPP_%02d"), i),
            FString::Printf(TEXT("Which word means the opposite of '%s'?"), OppositeWords[i][0]),
            TArray<FString>{OppositeWords[i][1], OppositeWords[(i + 3) % 20][1], OppositeWords[(i + 7) % 20][1], OppositeWords[(i + 11) % 20][1]},
            0, TEXT("Vocabulary"));
    }

    const TCHAR* Subjects[] = {TEXT("Aina"), TEXT("Ravi"), TEXT("Mei Lin"), TEXT("Daniel"), TEXT("Sara"), TEXT("Amir"), TEXT("Hana"), TEXT("Kumar")};
    struct FVerbSet { const TCHAR* Base; const TCHAR* Third; const TCHAR* Past; const TCHAR* Ing; const TCHAR* Tail; };
    const FVerbSet Verbs[] = {
        {TEXT("walk"), TEXT("walks"), TEXT("walked"), TEXT("walking"), TEXT("to school every morning")},
        {TEXT("eat"), TEXT("eats"), TEXT("ate"), TEXT("eating"), TEXT("breakfast at seven o'clock")},
        {TEXT("play"), TEXT("plays"), TEXT("played"), TEXT("playing"), TEXT("badminton after school")},
        {TEXT("drink"), TEXT("drinks"), TEXT("drank"), TEXT("drinking"), TEXT("milk in the morning")},
        {TEXT("wash"), TEXT("washes"), TEXT("washed"), TEXT("washing"), TEXT("the dishes after dinner")},
        {TEXT("carry"), TEXT("carries"), TEXT("carried"), TEXT("carrying"), TEXT("a bag to school")},
        {TEXT("watch"), TEXT("watches"), TEXT("watched"), TEXT("watching"), TEXT("television in the evening")},
        {TEXT("visit"), TEXT("visits"), TEXT("visited"), TEXT("visiting"), TEXT("the library every Saturday")}
    };

    for (int32 s = 0; s < 8; ++s)
    {
        for (int32 v = 0; v < 8; ++v)
        {
            AddGeneratedQuestion(FString::Printf(TEXT("GRAMMAR_PRESENT_%02d_%02d"), s, v),
                FString::Printf(TEXT("%s ___ %s."), Subjects[s], Verbs[v].Tail),
                TArray<FString>{Verbs[v].Base, Verbs[v].Third, Verbs[v].Past, Verbs[v].Ing}, 1, TEXT("Grammar"));

            AddGeneratedQuestion(FString::Printf(TEXT("TENSE_PAST_%02d_%02d"), s, v),
                FString::Printf(TEXT("Yesterday, %s ___ %s."), Subjects[s], Verbs[v].Tail),
                TArray<FString>{Verbs[v].Base, Verbs[v].Third, Verbs[v].Past, Verbs[v].Ing}, 2, TEXT("Tenses"));
        }
    }

    struct FConjunctionItem { const TCHAR* Prompt; const TCHAR* Correct; const TCHAR* A; const TCHAR* B; const TCHAR* C; };
    const FConjunctionItem Conjunctions[] = {
        {TEXT("I was thirsty, ___ I drank some water."), TEXT("so"), TEXT("but"), TEXT("because"), TEXT("or")},
        {TEXT("Mira was tired, ___ she finished her homework."), TEXT("but"), TEXT("so"), TEXT("because"), TEXT("or")},
        {TEXT("We stayed inside ___ it was raining."), TEXT("because"), TEXT("but"), TEXT("or"), TEXT("so")},
        {TEXT("Would you like rice ___ noodles?"), TEXT("or"), TEXT("because"), TEXT("so"), TEXT("but")},
        {TEXT("The bag is small ___ heavy."), TEXT("but"), TEXT("so"), TEXT("because"), TEXT("or")},
        {TEXT("He practised every day, ___ he improved."), TEXT("so"), TEXT("but"), TEXT("or"), TEXT("because")},
        {TEXT("I wore a jacket ___ the weather was cold."), TEXT("because"), TEXT("so"), TEXT("but"), TEXT("or")},
        {TEXT("You may read a book ___ draw a picture."), TEXT("or"), TEXT("because"), TEXT("so"), TEXT("but")},
        {TEXT("Sara is young ___ very responsible."), TEXT("but"), TEXT("because"), TEXT("or"), TEXT("so")},
        {TEXT("The road was flooded, ___ we took another route."), TEXT("so"), TEXT("but"), TEXT("because"), TEXT("or")},
        {TEXT("I closed the window ___ the rain was coming in."), TEXT("because"), TEXT("but"), TEXT("or"), TEXT("so")},
        {TEXT("We can walk ___ take the bus."), TEXT("or"), TEXT("so"), TEXT("because"), TEXT("but")},
        {TEXT("The test was difficult, ___ I tried my best."), TEXT("but"), TEXT("because"), TEXT("so"), TEXT("or")},
        {TEXT("Ravi forgot his umbrella, ___ he got wet."), TEXT("so"), TEXT("but"), TEXT("or"), TEXT("because")},
        {TEXT("The baby cried ___ it was hungry."), TEXT("because"), TEXT("but"), TEXT("so"), TEXT("or")},
        {TEXT("Would you like an apple ___ a banana?"), TEXT("or"), TEXT("because"), TEXT("but"), TEXT("so")},
        {TEXT("The room is old ___ clean."), TEXT("but"), TEXT("because"), TEXT("so"), TEXT("or")},
        {TEXT("She saved her money, ___ she could buy the book."), TEXT("so"), TEXT("but"), TEXT("or"), TEXT("because")},
        {TEXT("We were quiet ___ the baby was sleeping."), TEXT("because"), TEXT("so"), TEXT("or"), TEXT("but")},
        {TEXT("You can stay here ___ come with us."), TEXT("or"), TEXT("so"), TEXT("because"), TEXT("but")}
    };

    for (int32 i = 0; i < 20; ++i)
    {
        AddGeneratedQuestion(FString::Printf(TEXT("CONJ_%02d"), i), Conjunctions[i].Prompt,
            TArray<FString>{Conjunctions[i].Correct, Conjunctions[i].A, Conjunctions[i].B, Conjunctions[i].C}, 0, TEXT("Conjunctions"));
    }

    const TCHAR* InstructionSentences[] = {
        TEXT("Please open the window."), TEXT("Turn to page ten."), TEXT("Wash your hands before eating."), TEXT("Stand in a straight line."),
        TEXT("Write your name at the top."), TEXT("Put the book on the table."), TEXT("Listen carefully to the teacher."), TEXT("Close the door quietly."),
        TEXT("Bring your pencil tomorrow."), TEXT("Colour the picture neatly."), TEXT("Read the passage twice."), TEXT("Circle the correct answer."),
        TEXT("Keep your classroom clean."), TEXT("Wait for your turn."), TEXT("Raise your hand before speaking."), TEXT("Pack your school bag."),
        TEXT("Drink plenty of water."), TEXT("Walk slowly on the stairs."), TEXT("Check your work carefully."), TEXT("Share the crayons with your friend.")
    };

    for (int32 i = 0; i < 20; ++i)
    {
        AddGeneratedQuestion(FString::Printf(TEXT("TYPE_%02d"), i),
            FString::Printf(TEXT("What type of sentence is this?  %s"), InstructionSentences[i]),
            TArray<FString>{TEXT("instruction"), TEXT("question"), TEXT("exclamation"), TEXT("statement")}, 0, TEXT("Sentence Types"));
    }

    const TCHAR* PunctuationPrompts[] = {
        TEXT("Where is my pencil"), TEXT("What a beautiful rainbow"), TEXT("Please sit down"), TEXT("Did you finish your homework"),
        TEXT("That was amazing"), TEXT("The library closes at five"), TEXT("Why are you laughing"), TEXT("Watch out"),
        TEXT("My brother likes football"), TEXT("Can I borrow your ruler"), TEXT("What a huge elephant"), TEXT("We are visiting Ipoh tomorrow"),
        TEXT("Who is at the door"), TEXT("How exciting"), TEXT("The pupils are reading quietly"), TEXT("When does the bus arrive"),
        TEXT("Be careful"), TEXT("The cat is sleeping on the sofa"), TEXT("Which book do you want"), TEXT("What a wonderful day")
    };
    const TCHAR* CorrectMarks[] = {
        TEXT("?"), TEXT("!"), TEXT("."), TEXT("?"), TEXT("!"), TEXT("."), TEXT("?"), TEXT("!"), TEXT("."), TEXT("?"),
        TEXT("!"), TEXT("."), TEXT("?"), TEXT("!"), TEXT("."), TEXT("?"), TEXT("!"), TEXT("."), TEXT("?"), TEXT("!")
    };

    for (int32 i = 0; i < 20; ++i)
    {
        TArray<FString> Marks = {TEXT("."), TEXT("?"), TEXT("!"), TEXT(",")};
        const FString CorrectMark = CorrectMarks[i];
        int32 CorrectIndex = Marks.IndexOfByKey(CorrectMark);
        if (CorrectIndex == INDEX_NONE)
        {
            Marks[0] = CorrectMark;
            CorrectIndex = 0;
        }

        AddGeneratedQuestion(FString::Printf(TEXT("PUNC_%02d"), i),
            FString::Printf(TEXT("Choose the correct punctuation: %s___"), PunctuationPrompts[i]),
            Marks, CorrectIndex, TEXT("Punctuation"));
    }
}
