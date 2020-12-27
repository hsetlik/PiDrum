/*
  ==============================================================================

    Sequencer.h
    Created: 22 Dec 2020 10:27:19pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RGBColor.h"
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

class Step : public juce::ShapeButton
{
public:
    Step(int length, int maxSub, int index);
    ~Step();
    void paintButton(juce::Graphics& g, bool mouseIsOver, bool mouseIsDown) override;
private:
    bool isNote;
    int maxSubdivision; // maximum number of times a full note length can be divided
    int factor; //share of a quarter note which this step occupies
    int indexInSequence; //index of this step in the sequence (measured in units of maxSubdivision);
    ColorCreator color;
    juce::Colour noteColorOff;
    juce::Colour noteColorOn;
    juce::Colour restColorOff;
    juce::Colour restColorOn;
};
