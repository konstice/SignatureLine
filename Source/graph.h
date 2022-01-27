/*
  ==============================================================================

    graph.h
    Created: 6 Jul 2020 12:28:41pm
    Author:  kks

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Graph
{
public:
    //==========================================================================
    Graph()
    {
        kurve = nullptr;
        xpos = 0.5;
        ypos = 0.5;
    };
    ~Graph()
    {
        free(kurve);
    };
    //==========================================================================
    float* reset(int size){
        kurve = (float*)malloc(size * sizeof(float));
    }
    void setSize(int num)
    {
        size = num;
    }

    //einzelner prozess --> fuer processing
    float process(float input, int art)
    {
        float output;
        switch (art) {
        case 0:
        {
            output = atan(ypos * input) / atan(ypos);
        }
        }
        return output;
    }

private:
    float* kurve;
    int size;
    float xpos, ypos;
    int art;
    /*enum type
    {
        standart = 0,
        lagrange,
        potenz
    };*/
};