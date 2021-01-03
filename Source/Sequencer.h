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

enum stepState
{
    restOff,
    restOn,
    noteOff,
    noteOn
};

const int LABELWIDTH = 80;

class Step : public juce::ShapeButton
{
public:
    Step(float length, int maxSub, int index);
    ~Step(){}
    void paintButton(juce::Graphics& g, bool mouseIsOver, bool mouseIsDown) override;
    void setMaxSub(int newMaxSub)
    {
        maxSubdivision = newMaxSub;
    }
    void setFactor(float value)
    {
        factor = value;
    }
    void toggleNote()
    {
        if(isNote)
        {
            isNote = false;
        }
        else
        {
            isNote = true;
        }
    }
    void setRestOffColor(juce::Colour col)
    {
        restColorOff = col;
    }
    int getMaxSub() {return maxSubdivision;}
    float getFactor()
    {
        return factor;
    }
    int lengthInSubDivs()
    {
        return(factor * maxSubdivision);
    }
    int getNumSubDivs()
    {
        return maxSubdivision * factor;
    }
    int getIndex() {return indexInSequence;}
    void setIndex(int index) {indexInSequence = index;}
    void incrementIndex() {indexInSequence += 1;}
    void decrementIndex() {indexInSequence -= 1;}
    void select() {isSelected = true;}
    void deselect() {isSelected = false;}
    void setState(stepState newState) {state = newState;}
    bool getIsSelected() {return isSelected;}
    bool isNote;
private:
    stepState state;
    bool isSelected;
    int maxSubdivision; // maximum number of times a full note length can be divided
    float factor; //share of a quarter note which this step occupies
    int indexInSequence;
    ColorCreator color;
    juce::Colour noteColorOff = juce::Colours::lightblue;
    juce::Colour noteColorOn = juce::Colours::lightblue;
    juce::Colour restColorOff = juce::Colours::lightblue;
    juce::Colour restColorOn = juce::Colours::lightblue;
};



class TrackLabelComponent : public juce::Component
{
public:
    TrackLabelComponent(analogVoice type) : voiceType(type)
    {
        setInterceptsMouseClicks(false, false);
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

class Track : public juce::Component, juce::Button::Listener
{
public:
    Track(int length, int minimumSubdiv, analogVoice type);
    ~Track() {}
    void resized() override;
    void paint(juce::Graphics& g) override;
    void buttonClicked(juce::Button* b) override;
    void updateStepState(Step* toUpdate);
    void updateSteps(int numSubdivsIntoSequence);
    void increaseSubdivision();
    void decreaseSubdivision();
    Step* stepAtXPos(int xPos);
    void mouseDown(const juce::MouseEvent& m) override;
    void mouseDrag(const juce::MouseEvent& m) override;
    void selectStep(Step* toSelect);
    void clearSelection();
    juce::MidiMessage getMidiMessage();
    std::vector<Step*> selectedSteps;
    juce::OwnedArray<Step> steps;
    bool hasCurrentNote;
private:
    juce::Colour highlight;
    ColorCreator color;
    TrackLabelComponent label;
    int sequenceLength;
    int maxSubdivision;
    analogVoice drumVoice;
    Step* currentStep;
};

class Sequence : public juce::Component, juce::HighResolutionTimer
{
public:
    Sequence(int sequenceLength, int maxSubDivs, int tempo);
    ~Sequence() {}
    void togglePlay()
    {
        if(isPlaying) isPlaying = false;
        else isPlaying = true;
    }
    void setTempo(int newTempo);
    void incrementIndex();
    void hiResTimerCallback() override;
    void resized() override;
    void paint(juce::Graphics& g) override;
    bool keyPressed(const juce::KeyPress &p) override;
    void mouseDown(const juce::MouseEvent& m) override;
    int getTempo()
    {
        return tempo;
    }
    Track* getSelectedTrack();
private:
    std::unique_ptr<juce::MidiOutput> midiOut;
    ColorCreator color;
    int maxDivIndex;
    int maxSubdivisions;
    int tempo;
    int sequenceLength;
    bool isPlaying;
    juce::OwnedArray<Track> tracks;
};
