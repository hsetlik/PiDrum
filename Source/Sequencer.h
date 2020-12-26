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

class Step
{
public:
    Step(int thisIndex, float thisLength) : noteLength(thisLength), stepIndex(thisIndex), active(false), current(false)
    {
       if(thisLength < 1.0f)
       {
           isTuplet = true;
       }
        else
        {
            isTuplet = false;
        }
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
    void toggleCurrent()
    {
        if(current)
        { current = false;}
        else
        {current = true;}
    }
    bool isCurrent() {return current;}
    bool isTuplet;
    float noteLength; //fraction of a sequence's default note size
    int stepIndex;
    bool active;
private:
    bool current;
};

class Track
{
public:
    Track(int length, int index) : trackIndex(index)
    {
        
    }
    Track(int length, int index, analogVoice voice) : isAnalog(true), trackIndex(index), drumVoice(voice)
    {
    }
    ~Track()
    {
        
    }
    void makeTuplet(int stepIndex, int numDivisions);
    analogVoice getAnalogVoice()
    {
        return drumVoice;
    }
    bool isAnalog;
    std::vector<Step*> steps;
private:
    int trackIndex;
    analogVoice drumVoice;
};
