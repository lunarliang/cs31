#include <iostream>
#include <string>
#include <cctype>
#include <cassert>
using namespace std;

bool isTuneWellFormed(string tune)
{
    int k = 0; // k is our string character counter, so that the while loop goes through every character in the string
    while (k < tune.size())
    {
        if (tune.at(k) == 'A' || tune.at(k) == 'B' || tune.at(k) == 'C' || tune.at(k) == 'D' || tune.at(k) == 'E' || tune.at(k) == 'F' || tune.at(k) == 'G')
            k++;
        else if (tune.at(k) == '#' || tune.at(k) == 'b') // if the character is an accidental, character before it must be a letter
        {
            if (k > 0)
            {
                if (tune.at(k-1) == 'A' || tune.at(k-1) == 'B' || tune.at(k-1) == 'C' || tune.at(k-1) == 'D' || tune.at(k-1) == 'E' || tune.at(k-1) == 'F' || tune.at(k-1) == 'G')
                    k++;
            }
            else
            {
                return false;
            }
        }
        else if (isdigit(tune.at(k)))
        {
            if (k > 0)
            {
                if (tune.at(k - 1) == 'A' || tune.at(k - 1) == 'B' || tune.at(k - 1) == 'C' || tune.at(k - 1) == 'D' || tune.at(k - 1) == 'E' || tune.at(k - 1) == 'F' || tune.at(k - 1) == 'G' || tune.at(k - 1) == '#' || tune.at(k - 1) == 'b') // if character is number, then the character before it must be accidental or letter
                    k++;
            }
        }
        else if (tune.at(k) == '/')
            k++;
        else
        {
            return false;
        }
    }
    return true;
}

char translateNote(int octave, char noteLetter, char accidentalSign)
{
    // This check is here solely to report a common CS 31 student error.
    if (octave > 9)
    {
        cerr << "********** translateNote was called with first argument = "
            << octave << endl;
    }

    // Convert Cb, C, C#/Db, D, D#/Eb, ..., B, B#
    //      to -1, 0,   1,   2,   3, ...,  11, 12

    int note;
    switch (noteLetter)
    {
    case 'C':  note = 0; break;
    case 'D':  note = 2; break;
    case 'E':  note = 4; break;
    case 'F':  note = 5; break;
    case 'G':  note = 7; break;
    case 'A':  note = 9; break;
    case 'B':  note = 11; break;
    default:   return ' ';
    }
    switch (accidentalSign)
    {
    case '#':  note++; break;
    case 'b':  note--; break;
    case ' ':  break;
    default:   return ' ';
    }

    // Convert ..., A#1, B1, C2, C#2, D2, ...
    //      to ..., -2,  -1, 0,   1,  2, ...

    int sequenceNumber = 12 * (octave - 2) + note;

    const string keymap = "1!2@34$5%6^78*9(0qQwWeErtTyYuiIoOpPasSdDfgGhHjJklLzZxcCvVbBnm";
    if (sequenceNumber < 0 || sequenceNumber >= keymap.size())
        return ' ';
    return keymap.at(sequenceNumber);
}

int translateTune(string tune, string& instructions, int& badTick)
{
    if (!isTuneWellFormed(tune))
        return 1;

    string currentInstructions = "";
    int currentTick = 1;
    int k = 0;

    while (k < tune.size())
    {
		string notesInThisTick = ""; // to put all characters for a note in a single place

        // process everything inside a single tick
        while (k < tune.size() && tune.at(k) != '/')
        {
            // reset for a new note
            char noteLetter = tune.at(k);
            char accidentalSign = ' ';
            int octave = 4; // default is 4
            k++;

            // check if next char is an accidental
            if (k < tune.size() && (tune.at(k) == '#' || tune.at(k) == 'b'))
            {
                accidentalSign = tune.at(k);
                k++;
            }

            // check if next char is a digit
            if (k < tune.size() && isdigit(tune.at(k)))
            {
				octave = tune.at(k) - '0'; // convert char digit to int
                k++;
            }

			// translate note and check if it's playable
            char result = translateNote(octave, noteLetter, accidentalSign);

            if (result == ' ') // translateNote returns ' ' if unplayable
            {
                badTick = currentTick;
                return 2;
            }
            notesInThisTick += result;
        }

        // ending a tick
        if (notesInThisTick.empty())
        {
            currentInstructions += ' ';
        }
        else if (notesInThisTick.size() == 1)
        {
			currentInstructions += notesInThisTick; // adding the single note to the instructions
        }
        else
        {
            currentInstructions += '[' + notesInThisTick + ']'; // for chords
        }

        k++; // move past the slash
        currentTick++;
    }

    instructions = currentInstructions;
    return 0;
}

// main function, which just tests whether the 2 functions i wrote works or not
    int main()
{
    assert(isTuneWellFormed("D5//D/"));
    assert(!isTuneWellFormed("D5//Z/"));
    string instrs;
    int badt;
    instrs = "???"; badt = -999; // so we can detect whether these get changed
    assert(translateTune("D5//D/", instrs, badt) == 0 && instrs == "d y" && badt == -999);
    instrs = "???"; badt = -999; // so we can detect whether these get changed
    assert(translateTune("D5//Z/", instrs, badt) == 1 && instrs == "???" && badt == -999);
    assert(translateTune("D5//D8/", instrs, badt) == 2 && instrs == "???" && badt == 3);
        cerr << "All tests succeeded" << endl;
}

