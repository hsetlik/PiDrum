/*
  ==============================================================================

    Sequencer.h
    Created: 22 Dec 2020 10:27:19pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

enum analogVoice
{
    kick1,
    kick2,
    openHat,
    closedHat,
    snare,
    clap,
    clave
};

struct Step
{
    Step(int thisIndex, int thisLength) : length(thisLength), index(thisIndex), active(false)
    {
    }
    void toggle()
    {
        if(active)
        {
            active = false;
        } else
        {
            active = true;
        }
    }
    int length;
    int index;
    bool active;
};

class Track
{
public:
    Track(int length);
    Track(int length, analogVoice voice);
    ~Track();
    void makeTuplet(int stepIndex, int numDivisions);
    analogVoice getAnalogVoice()
    {
        return drumVoice;
    }
    bool isAnalog;
private:
    analogVoice drumVoice;
    juce::OwnedArray<Step> steps;
};

class Sequence
{
public:
    Sequence(int startRes = 4, int startLength = 16) : resolution(startRes), length(startLength)
    {
    }
    Sequence(int numDigitalTracks, int startRes = 4, int startLength = 16); //all analog tracks are created by default, work out digital track stuff later
    ~Sequence() {}
private:
    int resolution; //how long a default note is as a fraction (i.e. '4' indicates quarter notes, '8' for eigth notes etc)
    int length; //number of steps of the default length in the sequence
    juce::OwnedArray<Track> tracks;
};
