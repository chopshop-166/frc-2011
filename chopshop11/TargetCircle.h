#ifndef _TARGET_H
#define _TARGET_H

#include <vector>
#include "Vision/HSLImage.h"

// constants
#define PI 3.14159265358979
#define MINIMUM_SCORE 0.01 // minimum score to decide its a target

class TargetCircle
{
public:
    double m_majorRadius;  //ellipse major axis
    double m_minorRadius;  //ellipse minor axis
    double m_rawScore;
    double m_xPos;
    double m_yPos;
    double m_score;
    double m_rotation;		// ellipse rotation
    double m_xMax;

    static vector<TargetCircle> FindCircularTargets(HSLImage *image);
    double GetHorizontalAngle();
    double GetVerticalAngle();
    double GetSize();
    void Print();
};

#endif
