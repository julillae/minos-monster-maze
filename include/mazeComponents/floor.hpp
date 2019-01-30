#pragma once

#include "fixedComponent.hpp"

class Floor : public FixedComponent
{
    static Texture texture;

public:

    bool init();

    bool loadTexture();
    void setScaleDirection();
};