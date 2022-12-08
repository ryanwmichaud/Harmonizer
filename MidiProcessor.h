/*
  ==============================================================================

    MidiProcessor.h
    Created: 3 Dec 2022 12:32:56pm
    Author:  Ryan Michaud

    I moved all the fun processing code to this new class so you can see what I actually did outside of the plugin stuff
    
    I created an instance of this class and when it wants to process an audio block, it calls this class's process function
    and gives it a reference to the midi buffer so we can change it in memory
    
    This class also has a member processedBuffer that is a new buffer with the altered midi. I replace the buffer with 
    this in every block that gets processed.


    which channel? 
    keyboard state object w listeners?
    auto type? might be dif in the code dif type of message?
    how did I get samplePos and stuff? just initialized...

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class MidiProcessor {
    /*
    const std::vector<int> i1 = { -5,-7,-8 }; 
    const std::vector<int> i2 = { -2,-3,-7};
    const std::vector<int> i3 = { -1,-5,-10};
    const std::vector<int> i4 = { -4,-9,-11 };
    const std::vector<std::vector<int>> voicings = {i1,i2,i3,i4};
    */
    const std::vector<int> i1 = { -3,-5,-7,-8 };
    const std::vector<int> i2 = { -4,-7,-9,-11 };
    const std::vector<int> i3 = { -2,-3,-7,-10 };
    const std::vector<int> i4 = { -1,-5,-8,-10 };
    const std::vector<int> i5 = { -2,-4,-5,-9 };

    const std::vector<int> i6 = { -5,-7,-8 };
    const std::vector<int> i7 = { -2,-3,-7 };
    const std::vector<int> i8 = { -1,-5,-10 };
    const std::vector<int> i9 = { -4,-9,-11 };

    const std::vector<std::vector<int>> voicings = { i1,i2,i3,i4,i5,i6,i7,i8,i9 };
    
    const int limit = 9;
    int counter = 0;
    

public:

    void process(juce::MidiBuffer& midiMessages) {      //pass in midi buffer by reference so we can change it in memory

        processedBuffer.clear();                        //processed buffer is a class member so it keeps what we give it outside 
                                                        //the scope of this so we need to clear it
        processMidiInput(midiMessages);

        midiMessages.swapWith(processedBuffer);         //swap reference to midi buffer w processed buffer
    }



    void processMidiInput(const juce::MidiBuffer& midiMessages) {

        juce::MidiBuffer::Iterator it(midiMessages);    //use this iterator to iterate through midi events in the block
        juce::MidiMessage currentMessage;
        int samplePos;                                  //midi events can happen at the same sample position 
                                                        //and there can be no events at a sample position so we need to keep track of it

        while (it.getNextEvent(currentMessage, samplePos)) {

            
            

            if (currentMessage.isNoteOnOrOff()) {                   //only look at notes

                DBG(currentMessage.getDescription());

                if (currentMessage.isNoteOn()) {
                    counter = (counter + 1) % 5;
                    for (int i = 0; i <= 127; i++) {
                        auto off = juce::MidiMessage::noteOff(currentMessage.getChannel(),i,(float)0.5);
                        processedBuffer.addEvent(off, samplePos);
                    }
                }
                harmonize(currentMessage, samplePos);
            }
            else {
                processedBuffer.addEvent(currentMessage, samplePos);    //add to the processed buffer at the same time as original note
            }

        }

    }

    void harmonize(juce::MidiMessage currentMessage, int samplePos) {
        processedBuffer.addEvent(currentMessage, samplePos);
        
        for (int i = 0; i < voicings[counter].size(); i++) {
            auto transposedMessage = currentMessage;
            transposedMessage.setNoteNumber(currentMessage.getNoteNumber() + voicings[counter][i]);
            DBG(transposedMessage.getDescription());
            processedBuffer.addEvent(transposedMessage, samplePos);
        }
        
        
    }

    juce::MidiBuffer processedBuffer;  //class member. this is gonna hold our temporary altered notes at any given time before swapping

};
