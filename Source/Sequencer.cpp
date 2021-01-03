/*
  ==============================================================================

    Sequencer.cpp
    Created: 22 Dec 2020 10:27:19pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "Sequencer.h"

Step::Step(float length, int maxSub, int index) : juce::ShapeButton("stepButton", restColorOff, restColorOff, noteColorOff), isSelected(false),
maxSubdivision(maxSub), factor(length), indexInSequence(index)
{
    state = restOff;
    isNote = false;
    noteColorOff = color.RGBColor(126, 0, 33);
    noteColorOn = color.RGBColor(255, 0, 66);
    restColorOff = color.RGBColor(70, 69, 75);
    restColorOn = color.RGBColor(33, 57, 195);
    setClickingTogglesState(true);
    shouldUseOnColours(true);
}

void Step::paintButton(juce::Graphics &g, bool mouseIsOver, bool mouseIsDown)
{
    switch(state)
    {
        case restOff:
        {
            g.setColour(restColorOff);
            break;
        }
        case restOn:
        {
            g.setColour(restColorOn);
            break;
        }
        case noteOff:
        {
            g.setColour(noteColorOff);
            break;
        }
        case noteOn:
        {
            g.setColour(noteColorOn);
            break;
        }
    }
    g.fillRect(getLocalBounds());
    auto border = getLocalBounds().expanded(1);
    if(isSelected)
    {
        g.setColour(juce::Colours::white);
    }
    else
    {
        g.setColour(juce::Colours::black);
    }
    g.drawRect(border, 2);
}

//=========================================================================

Track::Track(int length, int minimumSubDiv, analogVoice type) :  label(type), sequenceLength(length), maxSubdivision(minimumSubDiv), drumVoice(type)
{
    hasCurrentNote = false;
    for(int i = 0; i < sequenceLength; ++i)
    {
        steps.add(new Step(1, maxSubdivision, i));
        addAndMakeVisible(steps.getLast());
        steps.getLast()->addListener(this);
        steps.getLast()->addMouseListener(this, true);
    }
    //addAndMakeVisible(&selectArea);
    addAndMakeVisible(&label);
    currentStep = steps.getFirst();
    highlight = color.RGBColor(255, 255, 255);
    setInterceptsMouseClicks(true, true);
    //addMouseListener(this, false);
}

void Track::paint(juce::Graphics &g)
{
    /*
    auto bounds = currentStep->getBoundsInParent();
    g.setColour(highlight);
    g.fillRect(bounds);
     */
}

void Track::resized()
{
    label.setBounds(0, 0, LABELWIDTH, getHeight());
    
    auto rightEdge = (float)LABELWIDTH + 0.0f;
    auto totalWidth = (float)getWidth() - rightEdge;
    auto subDivWidth = (float)(totalWidth / sequenceLength) / (double)maxSubdivision;
    //selectArea.setBounds(LABELWIDTH, 0, totalWidth, getHeight());
   // printf("Total width: %d\n", totalWidth);
    //printf("num subdivisions: %d\n", (sequenceLength * maxSubdivision));
    for(int i = 0; i < steps.size(); ++i)
    {
        auto width = ceil(steps.getUnchecked(i)->getNumSubDivs()) * (subDivWidth);
        steps.getUnchecked(i)->setBounds(rightEdge, 0.0f, width, (float)getHeight());
        rightEdge += width;
    }
}

void Track::buttonClicked(juce::Button *b)
{
    Step* thisStep = dynamic_cast<Step*>(b);
    thisStep->toggleNote();
    thisStep->repaint();
}

void Track::updateStepState(Step *toUpdate)
{
    if(toUpdate-> isNote)
    {
        if(toUpdate == currentStep)
        {
            toUpdate->setState(noteOn);
        }
        else
        {
            toUpdate->setState(noteOff);
        }
    }
    else
    {
        if(toUpdate == currentStep)
        {
            toUpdate->setState(restOn);
        }
        else
        {
            toUpdate->setState(restOff);
        }
    }
}

juce::MidiMessage Track::getMidiMessage()
{
    auto channel = 1;
    auto velocity = 1.0f;
    int noteNumber;
    switch(drumVoice)
    {
        case kick1:
        {
            noteNumber = 36;
            break;
        }
        case kick2: {
            noteNumber = 35;
            break;
        }
        case openHat: {
            noteNumber = 46;
            break;
        }
        case closedHat: {
            noteNumber = 42;
            break;
        }
        case snare: {
            noteNumber = 40;
            break;
        }
        case clap: {
            noteNumber = 39;
            break;
        }
        case clave: {
            noteNumber = 37;
            break;
        }
    }
    return juce::MidiMessage::noteOn(channel, noteNumber, velocity);
}

void Track::updateSteps(int numSubdivsIntoSequence)
{
    int pos = numSubdivsIntoSequence;
    int index = 0;
    for(int i = 0; i < steps.size(); ++i)
    {
        Step* checkStep = steps.getUnchecked(i);
        int noteEnd = index + checkStep->getNumSubDivs();
        if(index <= pos && pos < noteEnd)
        {
            if(currentStep != checkStep && checkStep->isNote)
            {
                hasCurrentNote = true;
            } else {
                hasCurrentNote = false;
            }
            currentStep = checkStep;
        }
        index = noteEnd;
        updateStepState(checkStep);
    }
}

Step* Track::stepAtXPos(int xPos)
{
    
    for(int i = 0; i < steps.size(); ++i)
    {
        auto thisX = steps.getUnchecked(i)->getScreenX();
        auto nextX = thisX + steps.getUnchecked(i)->getWidth();
        if( xPos > thisX && xPos <= nextX)
        {
            return steps.getUnchecked(i);
            break;
        }
    }
    return NULL;
}

void Track::clearSelection()
{
    if(selectedSteps.size() > 0)
    {
        for(int i = 0; i < selectedSteps.size(); ++i)
        {
            selectedSteps[i]->deselect();
        }
    }
    selectedSteps.clear();
}

void Track::selectStep(Step *toSelect)
{
    selectedSteps.push_back(toSelect);
    toSelect->select();
}

void Track::mouseDown(const juce::MouseEvent &m)
{
    clearSelection();
    //printf("track clicked\n");
}

void Track::mouseDrag(const juce::MouseEvent &m)
{
    if(m.mouseWasDraggedSinceMouseDown())
    {
        Step* selectedStep = stepAtXPos(m.getScreenX());
        if(selectedStep->getIsSelected() == false && selectedSteps.size() < 5)
        {
            selectStep(selectedStep);
        }
    }
}

void Track::increaseSubdivision()
{
  if(selectedSteps.size() > 0)
  {
      auto numNotesStart = (int)selectedSteps.size();
      steps.ensureStorageAllocated(steps.size() + 1);
      auto totalSubDivs = 0;
      auto firstNoteIndex = 1000;
      for(int note = 0; note < numNotesStart; ++note)
      {
          totalSubDivs += selectedSteps[note]->getNumSubDivs();
          auto index = selectedSteps[note]->getIndex();
          printf("start index is: %d\n", index);
          if(index < firstNoteIndex) {firstNoteIndex = index;}
          if(index < 0 )
          {
              printf("negative index\n");
          }
      }
      //inserting the new steps
      std::vector<Step*> newSteps;
      if(firstNoteIndex >= 0)
      {
          for(int i = 0; i < numNotesStart; ++i)
          {
              auto index = steps.indexOf(selectedSteps[i]);
              steps.remove(index);
          }
          auto numNotesEnd = numNotesStart + 1;
          auto newNoteSubDivs = totalSubDivs / numNotesEnd;
          auto newNoteFactor = ceil(newNoteSubDivs) / maxSubdivision;
          
          for(int i = 0; i < numNotesEnd; ++i)
          {
              auto writeIndex = firstNoteIndex + i;
              steps.insert(writeIndex, new Step(newNoteFactor, maxSubdivision, writeIndex));
              Step* lastStep = steps.getUnchecked(writeIndex);
              newSteps.push_back(lastStep);
              addAndMakeVisible(lastStep);
              lastStep->addListener(this);
              printf("Step %d is at: %d, %d\n", i, lastStep->getX(), lastStep->getY());
              printf("Step %d length: %d\n", i, lastStep->lengthInSubDivs());
              lastStep->addMouseListener(this, true);
          }
          resized();
      }
      int lastTupletIndex = firstNoteIndex + numNotesStart + 1;
      for(int i = 0; i < sequenceLength - lastTupletIndex; ++i)
      {
          steps.getUnchecked(lastTupletIndex + i)->incrementIndex();
      }
      //handling selection
      clearSelection();
      int numToSelect = 5;
      if(newSteps.size() < 5)
      {
          numToSelect = (int)newSteps.size();
      }
      for(int i = 0; i < numToSelect; ++i)
      {
          selectStep(newSteps[i]);
      }
  }
    
    printf("current step number: %d\n", steps.size());
}

void Track::decreaseSubdivision()
{
    
    if(selectedSteps.size() > 2)
    {
        auto numNotesStart = (int)selectedSteps.size();
        //steps.ensureStorageAllocated(steps.size() - 1);
        auto totalSubDivs = 0;
        auto firstNoteIndex = 1000;
        for(int note = 0; note < numNotesStart; ++note)
        {
            totalSubDivs += selectedSteps[note]->getNumSubDivs();
            auto index = selectedSteps[note]->getIndex();
            printf("start index is: %d\n", index);
            if(index < firstNoteIndex) {firstNoteIndex = index;}
            if(index < 0 )
            {
                printf("negative index\n");
            }
        }
        //inserting the new steps
        std::vector<Step*> newSteps;
        if(firstNoteIndex >= 0)
        {
            for(int i = 0; i < numNotesStart; ++i)
            {
                auto index = steps.indexOf(selectedSteps[i]);
                steps.remove(index);
            }
            auto numNotesEnd = numNotesStart - 1;
            auto newNoteSubDivs = totalSubDivs / numNotesEnd;
            auto newNoteFactor = ceil(newNoteSubDivs) / maxSubdivision;
            
            for(int i = 0; i < numNotesEnd; ++i)
            {
                auto writeIndex = firstNoteIndex + i;
                steps.insert(writeIndex, new Step(newNoteFactor, maxSubdivision, writeIndex));
                Step* lastStep = steps.getUnchecked(writeIndex);
                newSteps.push_back(lastStep);
                addAndMakeVisible(lastStep);
                resized();
                lastStep->addListener(this);
                printf("Step %d is at: %d, %d\n", i, lastStep->getX(), lastStep->getY());
                printf("Step %d length: %d\n", i, lastStep->lengthInSubDivs());
                lastStep->addMouseListener(this, true);
            }
            
        }
        //handling selection
        clearSelection();
        int numToSelect = 5;
        if(newSteps.size() < 5)
        {
            numToSelect = (int)newSteps.size();
        }
        for(int i = 0; i < numToSelect; ++i)
        {
            selectStep(newSteps[i]);
        }
        
    }
}



//==========================================================================================================================

Sequence::Sequence(int length, int maxSubDivs, int temp) : maxSubdivisions(maxSubDivs), tempo(temp), sequenceLength(length)
{
    setWantsKeyboardFocus(true);
    addMouseListener(this, true);
    
    juce::Array<juce::MidiDeviceInfo> allDevices = juce::MidiOutput::getAvailableDevices();
    auto midiId = allDevices[0].identifier;
    printf("Device identifier: %s\n", midiId.toUTF8());
    midiOut = juce::MidiOutput::openDevice(midiId);
    if(midiOut != NULL)
        {
            printf("Teensy Midi Found\n");
            
        }
    //setInterceptsMouseClicks(false, true);
    tracks.add(new Track(sequenceLength, maxSubDivs, kick1));
    tracks.add(new Track(sequenceLength, maxSubDivs, kick2));
    tracks.add(new Track(sequenceLength, maxSubDivs, openHat));
    tracks.add(new Track(sequenceLength, maxSubDivs, closedHat));
    tracks.add(new Track(sequenceLength, maxSubDivs, snare));
    tracks.add(new Track(sequenceLength, maxSubDivs, clap));
    tracks.add(new Track(sequenceLength, maxSubDivs, clave));
    
    for(int i = 0; i < tracks.size(); ++i)
    {
        addAndMakeVisible(*tracks.getUnchecked(i));
    }
    maxDivIndex = 0;
    isPlaying = false;
    auto msPerBeat = (60.0f / tempo) * 250.0f;
    auto msPerDiv = msPerBeat / maxSubdivisions;
    startTimer(msPerDiv);
}

void Sequence::setTempo(int newTempo)
{
    tempo = newTempo;
    stopTimer();
    auto msPerBeat = (60.0f / tempo) * 250.0f;
    auto msPerDiv = msPerBeat / maxSubdivisions;
    startTimer(msPerDiv);
}
void Sequence::hiResTimerCallback()
{
    if(isPlaying)
    {
        incrementIndex();
    }
    for(int i = 0; i < tracks.size(); ++i)
    {
        tracks.getUnchecked(i)->updateSteps(maxDivIndex);
        if(tracks.getUnchecked(i)->hasCurrentNote && midiOut != NULL)
        {
            midiOut->sendMessageNow(tracks.getUnchecked(i)->getMidiMessage());
        }
    }
}

void Sequence::incrementIndex()
{
    if(maxDivIndex < (maxSubdivisions * sequenceLength))
    {
        maxDivIndex += 1;
    }
    else
    {
        maxDivIndex = 0;
    }
}

void Sequence::resized()
{
    int trackHeight = getHeight() / tracks.size();
    for(int i = 0; i < tracks.size(); ++i)
    {
        tracks.getUnchecked(i)->setBounds(0, trackHeight * i, getWidth(), trackHeight);
    }
}

void Sequence::mouseDown(const juce::MouseEvent &m)
{
    for(int i = 0; i < tracks.size(); ++i)
    {
        tracks.getUnchecked(i)->clearSelection();
    }
}

void Sequence::paint(juce::Graphics &g)
{
    if(hasKeyboardFocus(false) == false)
    {
        juce::Timer::callAfterDelay (100, [&] { grabKeyboardFocus(); });
    }
}

Track* Sequence::getSelectedTrack()
{
    for(int i = 0; i < tracks.size(); ++i)
    {
        if(tracks.getUnchecked(i)->selectedSteps.size() > 0) return tracks.getUnchecked(i);
    }
    return NULL;
}

bool Sequence::keyPressed(const juce::KeyPress &p)
{
    auto key = p.getTextCharacter();
    switch(key)
    {
        case 'p': //p for play
        {
            togglePlay();
            break;
        }
        case 'f': //f for faster
        {
            setTempo(tempo + 1);
            break;
        }
        case 's': //s for slower
        {
            setTempo(tempo - 1);
            break;
        }
        case 'm': //m for more
        {
            Track* selectedTrack = getSelectedTrack();
            if(selectedTrack != NULL)
            {
                selectedTrack->increaseSubdivision();
            }
            else
            {
                printf("no valid track found\n");
            }
            break;
        }
        case 'l':
        {
            Track* selectedTrack = getSelectedTrack();
            if(selectedTrack != NULL)
            {
                selectedTrack->decreaseSubdivision();
            }
            else
            {
                printf("no valid track found\n");
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return false;
}

