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
    NoteButton(int thisIndex, float thisLength, juce::Colour setOnColor, juce::Colour setOffColor, juce::Colour setCurrentColor) :
    juce::ShapeButton("noteButton", juce::Colours::black, juce::Colours::black, juce::Colours::black),
    onColor(setOnColor),
    offColor(setOffColor),
    currentColor(setCurrentColor),
    seqStep(thisIndex, thisLength)
    {
        setWantsKeyboardFocus(false);
        edgeColor = juce::Colours::lightgrey;
        setClickingTogglesState(true);
    }
    ~NoteButton() {}
    void setOnColor(juce::Colour newColor)
    {
        onColor = newColor;
    }
    void paintButton(juce::Graphics& g, bool mouseOver, bool mouseDown) override
    {
        if(seqStep.active)
        {
            g.setColour(onColor);
        } else if(seqStep.isCurrent()) {
            g.setColour(currentColor);
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
    juce::Colour currentColor;
    Step seqStep;
};

const int LABELWIDTH = 80;

class TrackLabelComponent : public juce::Component
{
public:
    TrackLabelComponent(analogVoice type) : voiceType(type)
    {
        setWantsKeyboardFocus(false);
        bkgnd = color.RGBColor(70, 69, 75);
        switch(type)
        {
            case kick1:
            {
                image = juce::ImageCache::getFromMemory(BinaryData::kick10_5x_png, 1452);
                imgW = 51;
                imgH = 51;
                break;
            }
            case kick2:
            {
                image = juce::ImageCache::getFromMemory(BinaryData::kick20_5x_png, 1453);
                imgW = 51;
                imgH = 51;
                break;
            }
            case openHat:
            {
                image = juce::ImageCache::getFromMemory(BinaryData::openHat0_5x_png, 1195);
                imgW = 67;
                imgH = 38;
                break;
            }
            case closedHat:
            {
                image = juce::ImageCache::getFromMemory(BinaryData::closedHat0_5x_png, 1072);
                imgW = 67;
                imgH = 38;
                break;
            }
            case snare:
            {
                image = juce::ImageCache::getFromMemory(BinaryData::snare0_5x_png, 1717);
                imgW = 63;
                imgH = 53;
                break;
            }
            case clave:
            {
                image = juce::ImageCache::getFromMemory(BinaryData::clave0_5x_png, 1236);
                imgW = 55;
                imgH = 36;
                break;
            }
            case clap:
            {
                image = juce::ImageCache::getFromMemory(BinaryData::clap0_5x_png, 1629);
                imgW = 50;
                imgH = 58;
                break;
            }
        }
    }
    ~TrackLabelComponent() {}
    void paint(juce::Graphics& g) override
    {
        g.fillAll(bkgnd);
        g.setOpacity(1.0f);
        int destX = (getWidth() - imgW) / 2;
        int destY = (getHeight() - imgH) / 2;
        g.drawImage(image, destX, destY, imgW, imgH, 0, 0, imgW, imgH);
        juce::Rectangle<int> border = getLocalBounds();
        g.setColour(juce::Colours::black);
        g.drawRect(border);
    }
private:
    int imgW;
    int imgH;
    ColorCreator color;
    juce::Colour bkgnd;
    analogVoice voiceType;
    juce::Image image;
};

class TrackComponent : public juce::Component
{
public:
    TrackComponent(int length, int index) : trackLabel(kick1), track(length, index)
    {
        setWantsKeyboardFocus(false);
        stepOn = color.RGBColor(190, 1, 49);
        stepOff = color.RGBColor(126, 0, 33);
        stepCurrent = color.RGBColor(254, 0, 66);
        for(int i = 0; i < length; ++i)
        {
            stepButtons.add(new NoteButton(i, 1.0f, stepOn, stepOff, stepCurrent));
            NoteButton* newestStep = stepButtons.getLast();
            addAndMakeVisible(newestStep);
            track.steps.push_back(newestStep->getStep());
        }
        setCurrentStep(0);
    }
    TrackComponent(int length, int index, analogVoice drum) : trackLabel(drum), drumVoice(drum), track(length, index)
    {
        stepOn = color.RGBColor(190, 1, 49);
        stepOff = color.RGBColor(126, 0, 33);
        stepCurrent = color.RGBColor(254, 0, 66);
        addAndMakeVisible(trackLabel);
        for(int i = 0; i < length; ++i)
        {
            stepButtons.add(new NoteButton(i, 1.0f, stepOn, stepOff, stepCurrent));
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
        currentStep->toggleCurrent();
        currentStep = stepButtons.getUnchecked(indexOfTotalSteps)->getStep();
        currentStep->toggleCurrent();
    }
    void resized() override
    {
        int sequenceLength = stepButtons.size();
        trackLabel.setBounds(0, 0, LABELWIDTH, getHeight());
        int buttonWidth = (getWidth() - LABELWIDTH) / sequenceLength;
        for(int i = 0; i < sequenceLength; ++i)
        {
            stepButtons.getUnchecked(i)->setBounds((i * buttonWidth) + LABELWIDTH, 0, buttonWidth, getHeight());
        }
    }
    void paint(juce::Graphics& g) override
    {
        for(int i = 0; i < stepButtons.size(); ++i)
        {
            Step* toCheck = stepButtons.getUnchecked(i)->getStep();
            if(toCheck == currentStep)
            {
                stepButtons.getUnchecked(i)->setOnColor(stepCurrent);
            } else {
                stepButtons.getUnchecked(i)->setOnColor(stepOn);
            }
        }
    }
private:
    TrackLabelComponent trackLabel;
    analogVoice drumVoice;
    juce::Colour stepOn;
    juce::Colour stepOff;
    juce::Colour stepCurrent;
    juce::OwnedArray<NoteButton> stepButtons;
    ColorCreator color;
    Step* currentStep;
    Track track;
};

class SequencerPanel  : public juce::Component, juce::Button::Listener, juce::HighResolutionTimer
{
public:
    SequencerPanel(int numDigitalTracks, int length) : currentStepIndex(0), currentTempo(120), sequenceLength(length)
    {
        setAlwaysOnTop(true);
        playheadColor = color.RGBColor(255, 149, 0);
        setWantsKeyboardFocus(true);
        isPlaying = false;
        trackComponents.add(new TrackComponent(length, 0, kick1));
        trackComponents.add(new TrackComponent(length, 1, kick2));
        trackComponents.add(new TrackComponent(length, 2, openHat));
        trackComponents.add(new TrackComponent(length, 3, closedHat));
        trackComponents.add(new TrackComponent(length, 4, snare));
        trackComponents.add(new TrackComponent(length, 5, clave));
        trackComponents.add(new TrackComponent(length, 6, clap));
        for(int i = 0; i < trackComponents.size(); ++i)
        {
            addAndMakeVisible(trackComponents.getUnchecked(i));
            for(int button = 0; button < length; ++button)
            {
                NoteButton* currentStepButton = trackComponents.getUnchecked(i)->getStep(button);
                currentStepButton->addListener(this);
            }
        }
        int msInterval = (60 / currentTempo) * 1000;
        startTimer(msInterval);
        juce::Timer::callAfterDelay (100, [&]
        {
            grabKeyboardFocus();
        });
    }
    ~SequencerPanel()
    {
        
    }
    void incrementStep()
    {
        if(currentStepIndex < (sequenceLength - 1))
        {
            ++currentStepIndex;
        } else  {
            currentStepIndex = 0;
        }
        int numTracks = trackComponents.size();
        for(int i = 0; i < numTracks; ++i)
        {
            trackComponents.getUnchecked(i)->setCurrentStep(currentStepIndex);
        }
    }
    void hiResTimerCallback() override
    {
      if(isPlaying)
      {
          //advance the current step on each track
          incrementStep();
      }
    }
    void togglePlay()
    {
        if(isPlaying) {isPlaying = false;}
        else {isPlaying = true;}
    }
    bool keyPressed(const juce::KeyPress &k) override
    {
        if(k.getTextCharacter() == 'p')
        {
            togglePlay();
        }
        return false;
    }
    void buttonClicked(juce::Button* button) override
    {
        NoteButton* note = dynamic_cast<NoteButton*>(button);
        note->getStep()->toggle();
    }
    void paint (juce::Graphics& g) override
    {
        auto sequenceWidth = getWidth() - LABELWIDTH;
        auto sequenceHeight = getHeight();
        auto stepWidth = sequenceWidth / sequenceLength;
        auto stepX = LABELWIDTH + (currentStepIndex * stepWidth);
        juce::Rectangle<int> playhead {stepX, 0, stepWidth, sequenceHeight};
        g.setColour(playheadColor);
        g.drawRect(playhead);
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
    ColorCreator color;
    juce::Colour playheadColor;
    int currentStepIndex;
    int currentTempo;
    bool isPlaying;
    int sequenceLength;
    juce::OwnedArray<TrackComponent> trackComponents;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerPanel)
};
