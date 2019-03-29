#include "../include/gameSave.hpp"

Document GameSave::document;
Level* GameSave::gameLevel;

void GameSave::save_game(Level* level)
{
    gameLevel = level;

    ofstream saveFile("../src/savedGame.txt");

    document.SetObject();

    save_level();
    save_player();

    StringBuffer strbuf;
    Writer<StringBuffer> writer(strbuf);
    document.Accept(writer);

    saveFile << strbuf.GetString() << std::endl;
    saveFile.close();
}

void GameSave::save_player()
{
    // must pass an allocator when the object may need to allocate memory
    Document::AllocatorType& allocator = document.GetAllocator();

    Value x, y;
    x.SetFloat(gameLevel->m_player.m_position.x);
    y.SetFloat(gameLevel->m_player.m_position.y);

    // create a rapidjson object type
    Value position(rapidjson::kObjectType);
    position.AddMember("x", x, allocator);
    position.AddMember("y", y, allocator);
    document.AddMember("player_pos", position, allocator);
}

void GameSave::save_level()
{
    // must pass an allocator when the object may need to allocate memory
    Document::AllocatorType& allocator = document.GetAllocator();

    Value level, rotation, rotationDeg;
    level.SetInt(gameLevel->get_current_level());
    rotation.SetFloat(gameLevel->get_rotation());
    rotationDeg.SetFloat(gameLevel->get_rotationDeg());
    document.AddMember("level", level, allocator);
    document.AddMember("rotation", rotation, allocator);
    document.AddMember("rotationDeg", rotationDeg, allocator);
}

void GameSave::load_game()
{

    FILE* fp = fopen("../src/savedGame.txt", "r");
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    document.ParseStream(is);
    fclose(fp);


}