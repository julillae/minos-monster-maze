#pragma once

//stdlib
#include <fstream>
#include "../ext/rapidjson/document.h"
#include "../ext/rapidjson/writer.h"
#include "../ext/rapidjson/stringbuffer.h"
#include <iostream>
#include "../ext/rapidjson/filereadstream.h"
#include <cstdio>

//#include "level.hpp"

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
    void static save_enemies();
    void static save_spiders();
    void static save_harpies();
    void static save_minotaur();

};