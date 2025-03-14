#include "config_loader.h"
#include <iostream>

ConfigLoader::ConfigLoader(const std::string &filename) : filename_(filename)
{
    Load();
}

bool ConfigLoader::Load()
{
    try
    {
        config_ = YAML::LoadFile(filename_);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading config file: " << e.what() << std::endl;
        return false;
    }
}

float ConfigLoader::GetADSR(const std::string &param)
{
    return config_["adsr"][param] ? config_["adsr"][param].as<float>() : 0.1f;
}

float ConfigLoader::GetEffect(const std::string &param)
{
    return config_["effects"][param] ? config_["effects"][param].as<float>() : 0.5f;
}

