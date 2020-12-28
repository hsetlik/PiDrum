/*
  ==============================================================================

    SequencerPanel.h
    Created: 22 Dec 2020 10:26:27pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Sequencer.h"
#include "RGBColor.h"

//==============================================================================
/*
*/


class SequencerPanel : public juce::Component, juce::Timer
{
public:
    SequencerPanel(int length, int maxSub, int tempo) : seq(length, maxSub, tempo)
    {
        addAndMakeVisible(seq);
        startTimer(50);
    }
    ~SequencerPanel() {}
    void resized() override
    {
        seq.setBounds(getBounds());
    }
    void timerCallback() override
    {
        seq.repaint();
    }
private:
    Sequence seq;
};


