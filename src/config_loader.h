#pragma once
#include <string>
#include <yaml-cpp/yaml.h>

class ConfigLoader
{
public:
    ConfigLoader(const std::string &filename);
    bool Load(); // Load config from file
    float GetADSR(const std::string &param);
    float GetEffect(const std::string &param);

private:
    std::string filename_;
    YAML::Node config_;
};

