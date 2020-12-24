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
class NoteButton : public juce::ShapeButton
{
public:
    NoteButton(int thisIndex, float thisLength, juce::Colour setOnColor, juce::Colour setOffColor) :
    juce::ShapeButton("noteButton", setOnColor, setOffColor, setOffColor),
    onColor(setOnColor),
    offColor(setOffColor),
    seqStep(thisIndex, thisLength)
    {
        edgeColor = juce::Colours::lightgrey;
        setClickingTogglesState(true);
    }
    ~NoteButton() {}
    void paintButton(juce::Graphics& g, bool mouseOver, bool mouseDown) override
    {
        if(seqStep.active)
        {
            g.setColour(onColor);
        } else {
            g.setColour(offColor);
        }
        g.fillAll();
        g.setColour(edgeColor);
        juce::Rectangle<int> border = getLocalBounds();
        g.drawRect(border);
    }
    Step* getStep()
    {
        return &seqStep;
    }
private:
    juce::Colour edgeColor;
    juce::Colour onColor;
    juce::Colour offColor;
    Step seqStep;
};
class TrackComponent : public juce::Component
{
public:
    TrackComponent(int length, int index) : track(length, index)
    {
        stepOn = color.RGBColor(190, 1, 49);
        stepOff = color.RGBColor(126, 0, 33);
        stepCurrent = color.RGBColor(254, 0, 66);
        for(int i = 0; i < length; ++i)
        {
            stepButtons.add(new NoteButton(i, 1.0f, stepOn, stepOff));
            NoteButton* newestStep = stepButtons.getLast();
            addAndMakeVisible(newestStep);
            track.steps.push_back(newestStep->getStep());
        }
    }
    ~TrackComponent()
    {
        
    }
    NoteButton* getStep(int index)
    {
        return stepButtons.getUnchecked(index);
    }
    void setCurrentStep(int indexOfTotalSteps)
    {
    }
    void resized() override
    {
        int sequenceLength = stepButtons.size();
        int buttonWidth = getWidth() / sequenceLength;
        for(int i = 0; i < sequenceLength; ++i)
        {
            stepButtons.getUnchecked(i)->setBounds(i * buttonWidth, 0, buttonWidth, getHeight());
        }
    }
private:
    juce::Colour stepOn;
    juce::Colour stepOff;
    juce::Colour stepCurrent;
    juce::OwnedArray<NoteButton> stepButtons;
    ColorCreator color;
    Step* currentStep;
    Track track;
};

class SequencerPanel  : public juce::Component, juce::Button::Listener
{
public:
    SequencerPanel(int numTracks, int length) : sequenceLength(length)
    {
        for(int i = 0; i < numTracks; ++i)
        {
            trackComponents.add(new TrackComponent(length, i));
            addAndMakeVisible(trackComponents.getLast());
            for(int button = 0; button < length; ++button)
            {
                NoteButton* currentStepButton = trackComponents.getLast()->getStep(button);
                currentStepButton->addListener(this);
            }
        }
    }
    ~SequencerPanel()
    {
        
    }
    void buttonClicked(juce::Button* button) override
    {
        NoteButton* note = dynamic_cast<NoteButton*>(button);
        note->getStep()->toggle();
    }
    void paint (juce::Graphics&) override
    {
        
    }
    void resized() override
    {
        int numTracks = trackComponents.size();
        int trackHeight = getHeight() / numTracks;
        for(int i = 0; i < numTracks; ++i)
        {
            trackComponents.getUnchecked(i)->setBounds(0, i * trackHeight, getWidth(), trackHeight);
        }
    }

private:
    int sequenceLength;
    juce::OwnedArray<TrackComponent> trackComponents;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerPanel)
};
