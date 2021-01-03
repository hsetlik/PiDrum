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

const int HEADERHEIGHT = 45;
class SequenceHeader : public juce::Component
{
public:
    SequenceHeader(juce::String seqName, Sequence* sibling) : name(seqName), siblingSeq(sibling)
    {
        background = color.RGBColor(42, 58, 112);
    }
    ~SequenceHeader() {}
    void paint(juce::Graphics& g) override
    {
        g.fillAll(background);
        auto n = getHeight() / 6;
        g.setColour(text);
        g.setFont(juce::Font("Press Start 2P", 15.0f, 0));
        g.drawSingleLineText(name, n, 5 * n);
        auto tempoString = "Tempo: " + juce::String(siblingSeq->getTempo());
        auto x = getWidth() / 3;
        g.drawSingleLineText(tempoString, 2 * x, 5 * n);
    }
private:
    juce::Colour background = juce::Colours::black;
    juce::Colour text = juce::Colours::white;
    ColorCreator color;
    juce::String name;
    Sequence* siblingSeq;
};


class SequencerPanel : public juce::Component, juce::Timer
{
public:
    SequencerPanel(int length, int maxSub, int tempo) : seq(length, maxSub, tempo), header("untitled", &seq)
    {
        addAndMakeVisible(seq);
        addAndMakeVisible(header);
        setInterceptsMouseClicks(false, true);
        startTimerHz(20);
    }
    ~SequencerPanel() {}
    void resized() override
    {
        header.setBounds(0, 0, getWidth(), HEADERHEIGHT);
        seq.setBounds(0, HEADERHEIGHT, getWidth(), getHeight() - HEADERHEIGHT);
    }
    void timerCallback() override
    {
        seq.repaint();
        //header.repaint();
    }
private:
    Sequence seq;
    SequenceHeader header;
};


