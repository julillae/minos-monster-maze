#pragma once

//stdlib
#include <fstream>
#include "../ext/rapidjson/document.h"
#include "../ext/rapidjson/writer.h"
#include "../ext/rapidjson/stringbuffer.h"
#include <iostream>
#include "../ext/rapidjson/filereadstream.h"
#include <cstdio>

//internal
#include "../include/level.hpp"

using namespace rapidjson;

using namespace std;

class Level;

class GameSave
{
public:
    Document static document;
    Level static *gameLevel;

    void static save_game(Level* level);

    void static load_game();

private:
    void static save_player();
    void static save_level();

};