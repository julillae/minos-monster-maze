#include "../include/gameSave.hpp"
#include "../include/level.hpp"

Document GameSave::document;
Level* GameSave::gameLevel;

void GameSave::save_game(Level* level)
{
    gameLevel = level;

    ofstream saveFile("../src/savedGame.txt");

    document.SetObject();

    save_level();
    save_player();
    save_enemies();

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

    Value pos_x, pos_y, scale_x, alive;
    pos_x.SetFloat(gameLevel->m_player.m_position.x);
    pos_y.SetFloat(gameLevel->m_player.m_position.y);
    scale_x.SetFloat(gameLevel->m_player.m_scale.x);
    alive.SetBool(gameLevel->m_player.is_alive());

    // create a rapidjson object type
    Value player(rapidjson::kObjectType);
    player.AddMember("pos_x", pos_x, allocator);
    player.AddMember("pos_y", pos_y, allocator);
    player.AddMember("scale_x", scale_x, allocator);
    player.AddMember("alive", alive, allocator);

    document.AddMember("player", player, allocator);
}

void GameSave::save_minotaur()
{
    // must pass an allocator when the object may need to allocate memory
    Document::AllocatorType& allocator = document.GetAllocator();
    Minotaur m_minotaur = gameLevel->get_minotaur();
    m_minotaur.stopRotating();
    if (m_minotaur.characterState->currentState == preparing) {
        m_minotaur.characterState->changeState(m_minotaur.previous_state);
        m_minotaur.update(0.f);
    }

    Value pos_x, pos_y, scale_x, alive, vel_x;
    pos_x.SetFloat(m_minotaur.m_position.x);
    pos_y.SetFloat(m_minotaur.m_position.y);
    scale_x.SetFloat(m_minotaur.m_scale.x);
    alive.SetBool(m_minotaur.is_alive());
    vel_x.SetFloat(m_minotaur.get_velocity().x);

    // create a rapidjson object type
    Value minotaur(rapidjson::kObjectType);
    minotaur.AddMember("pos_x", pos_x, allocator);
    minotaur.AddMember("pos_y", pos_y, allocator);
    minotaur.AddMember("scale_x", scale_x, allocator);
    minotaur.AddMember("alive", alive, allocator);
    minotaur.AddMember("velocity_x",vel_x, allocator);

    document.AddMember("minotaur", minotaur, allocator);
}

void GameSave::save_level()
{
    // must pass an allocator when the object may need to allocate memory
    Document::AllocatorType& allocator = document.GetAllocator();

    Value level, rotation, rotationDeg, rotationEnergy;
    level.SetInt(gameLevel->get_current_level());
    rotation.SetFloat(gameLevel->get_rotation());
    rotationDeg.SetFloat(gameLevel->get_rotationDeg());
    rotationEnergy.SetFloat(gameLevel->get_rotationEnergy());
    document.AddMember("level", level, allocator);
    document.AddMember("rotation", rotation, allocator);
    document.AddMember("rotationDeg", rotationDeg, allocator);
    document.AddMember("rotationEnergy", rotationEnergy, allocator);
}

void GameSave::save_enemies()
{
    save_spiders();
    save_harpies();
    if (gameLevel->minotaurPresent) {
        save_minotaur();
    }
}

void GameSave::save_spiders()
{
    // must pass an allocator when the object may need to allocate memory
    Document::AllocatorType& allocator = document.GetAllocator();
    std::vector<Spider> m_spiders = gameLevel->get_spiders();

    {
        Value spiders(kArrayType);

        {
            for (auto& s : m_spiders) {
                Value spider(kObjectType);
                Value pos_x, pos_y, vel_x, vel_y, scale_x;
                pos_x.SetFloat(s.get_position().x);
                pos_y.SetFloat(s.get_position().y);
                vel_x.SetFloat(s.get_velocity().x);
                vel_y.SetFloat(s.get_velocity().y);
                scale_x.SetFloat(s.get_scale().x);
                spider.AddMember("pos_x", pos_x, allocator);
                spider.AddMember("pos_y", pos_y, allocator);
                spider.AddMember("vel_x", vel_x, allocator);
                spider.AddMember("vel_y", vel_y, allocator);
                spider.AddMember("scale_x", scale_x, allocator);
                spiders.PushBack(spider, allocator);

            }

            document.AddMember("spiders", spiders, allocator);

        }
    }
}

void GameSave::save_harpies()
{
    // must pass an allocator when the object may need to allocate memory
    Document::AllocatorType& allocator = document.GetAllocator();
    std::vector<Harpy> m_harpies = gameLevel->get_harpies();

    {
        Value harpies(kArrayType);

        {
            for (auto& h : m_harpies) {
                Value harpy(kObjectType);
                Value pos_x, pos_y, vel_x, vel_y, scale_x;
                pos_x.SetFloat(h.get_position().x);
                pos_y.SetFloat(h.get_position().y);
                vel_x.SetFloat(h.get_velocity().x);
                vel_y.SetFloat(h.get_velocity().y);
                scale_x.SetFloat(h.get_scale().x);
                harpy.AddMember("pos_x", pos_x, allocator);
                harpy.AddMember("pos_y", pos_y, allocator);
                harpy.AddMember("vel_x", vel_x, allocator);
                harpy.AddMember("vel_y", vel_y, allocator);
                harpy.AddMember("scale_x", scale_x, allocator);
                harpies.PushBack(harpy, allocator);

            }

            document.AddMember("harpies", harpies, allocator);

        }
    }
}

void GameSave::load_game()
{

    FILE* fp = fopen("../src/savedGame.txt", "r");
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    document.ParseStream(is);
    fclose(fp);


}