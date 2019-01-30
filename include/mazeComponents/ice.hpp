#pragma once

#include "fixedComponent.hpp"

class Ice : public FixedComponent
{
    static Texture texture;

public:

    bool init();

    bool loadTexture();
    void setScaleDirection();
};