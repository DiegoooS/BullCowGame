// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();

    // Transfer words from txt file into an array
    const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordsList/HiddenWordList.txt");

    // Get Isograms member variable(array) and declare it with words from HiddenWordList.txt after they fulfills certain conditions
    // [](const FString& Word){}  means lambda and is equivalent to the bool function 
    FFileHelper::LoadFileToStringArrayWithPredicate(Isograms, *WordListPath, [](const FString& Word) 
    {
        return (Word.Len() >= 4 && Word.Len() <= 8 && IsIsogram(Word));   // Find words with >=4 letters and <=8 letters AND Isograms
    });

    SetupGame();
}

void UBullCowCartridge::OnInput(const FString& PlayerInput) // When the player hits enter
{
    if (bGameOver) // Restart game if player guess a word or lost the game
    {
        ClearScreen();
        SetupGame();
        return;
    }

    ProcessGuess(PlayerInput); // Checking Player Guess
}

void UBullCowCartridge::SetupGame()
{   
    // Initializing Variables
    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num() - 1)]; 
    Lives = HiddenWord.Len() * 2;
    bGameOver = false;
    Levels = 1;

    // Welcoming The Player
    PrintLine(TEXT("Welcome in the Bulls&Cows Game!"));
    PrintLine(TEXT("\nPress TAB to use a console."));
    PrintLine(TEXT("Write your guess and press ENTER!"));

    PrintLine(TEXT("\nLEVEL: %i"), Levels);
    HiddenWordHint(HiddenWord); // Hint for the player
    PrintLine(TEXT("\nIts the %i letter word!"), HiddenWord.Len());
    PrintLine(TEXT("You have %i lives."), Lives);
    //PrintLine(TEXT("The HiddenWord is: %s."), *HiddenWord); // Debug Line
    // PrintLine(TEXT("Number of possible words: %i"), Isograms.Num()); // Debug Line
}

void UBullCowCartridge::EndGame()
{
    bGameOver = true;

    PrintLine(TEXT("\nPress ENTER to play again..."));
}

void UBullCowCartridge::ProcessGuess(const FString& Guess)
{
    if (Guess.Len() != HiddenWord.Len()) // Check Right Number Of Characters
    {
        PrintLine(TEXT("\nWrong number of characters.\nHidden Word is %i characters long."), HiddenWord.Len());
        return;
    }

    if (Guess == HiddenWord) // Check if guess is correct
    {
        ClearScreen();
        PrintLine(TEXT("\nGood job!"));
        NextLevel();
        return;
    }

    if(!IsIsogram(Guess)) // Check If guess is Isogram
    {
        PrintLine(TEXT("\nNo repeating letters. Try again!"), HiddenWord.Len());
        return;
    }

    --Lives;
    if (Lives <= 0) // Check if player lost all the lives
    {
        ClearScreen();
        PrintLine(TEXT("\nYou lose!\nCorrect isogram was: %s"), *HiddenWord);
        EndGame();
        return;
    }

    // ! Show Number Of Bulls And Cows
    // Search trough array letter by letter
    FBullCowCount Count = GetBullCows(Guess);

    PrintLine(TEXT("You have %i Bulls and %i Cows!"), Count.Bulls, Count.Cows);
    PrintLine(TEXT("\nWrong guess, try again! %i lives left!"), Lives);
}

bool UBullCowCartridge::IsIsogram(const FString& Word)
{
    for (int32 Index = 0; Index < Word.Len(); Index++)  // Loop trough all letters of the guess
    {
        for (int32 Comparision = Index + 1 ; Comparision < Word.Len() ; Comparision++) // Loop trough next letters of the guess
        {
            if (Word[Index] == Word[Comparision]) return false; // Compare letters. Return false if they are the same.
        }
    }
    return true; // Return true if letters repeating
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const
{
    FBullCowCount CountBullCows;

    for (int32 GuessIndex = 0; GuessIndex < Guess.Len(); GuessIndex++)
    {
        if(Guess[GuessIndex] == HiddenWord[GuessIndex])
        {
            ++CountBullCows.Bulls;
            continue;
        }

        for (int32 OtherLettersIndex = 0; OtherLettersIndex < HiddenWord.Len(); OtherLettersIndex++)
        {
            if (Guess[GuessIndex] == HiddenWord[OtherLettersIndex])
            {
                ++CountBullCows.Cows;
                break;
            }
        }
    }
    return CountBullCows;
}

void UBullCowCartridge::NextLevel() 
{
    ++Levels;
    if (Levels > 10)
    {
        ClearScreen();
        PrintLine(TEXT("You win!"));
        EndGame();
        return;
    }

    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num() - 1)];
     
    PrintLine(TEXT("\nLEVEL: %i\n"), Levels);

    if(Levels > 5)
    {
        Lives = HiddenWord.Len();
    }
    else
    {
        Lives = HiddenWord.Len() * 2;
        HiddenWordHint(HiddenWord);
    }

    if (Levels == 6)
    {
        PrintLine(TEXT("Its getting harder!\nHints disabled!\nLess life!"));
    }

    PrintLine(TEXT("Its the %i letter word!"), HiddenWord.Len());
    PrintLine(TEXT("You have %i lives."), Lives);
    //PrintLine(TEXT("The HiddenWord is: %s."), *HiddenWord); // Debug Line
}

void UBullCowCartridge::HiddenWordHint(const FString& Word) const
{
    FString WordHint;
    const int32 RandomLetter = FMath::RandRange(0, HiddenWord.Len() - 1);

    for (int32 Index = 0; Index < Word.Len(); Index++)
    {
        if(Index == RandomLetter)
        {
            WordHint.AppendChar // Add one letter as a hint
            (
                Word[Index]
            );
            continue;
        }
        WordHint.Append
        (
            "*" // Rest of the letters will be displayed as *
        );
    }
    PrintLine(TEXT("The hidden word is: %s"), *WordHint);
}


// --->  EXAMPLE OF USING FOR LOOP -> compiler figure out how many iterations you need.  <--- 

//     for (FString Word : WordsList)
//     {
//         if(Word.Len() >= 4)
//         {
//             ValidWords.Emplace(Word);
//         }
//     }
