#include "TopicsConfig.h"

std::map<std::string, uint8_t> TopicsConfig::_topics_list;
std::map<uint8_t, std::string> TopicsConfig::_identifiers_list;

void TopicsConfig::load_configuration()
{
  const char* env_path = std::getenv("ROS_ALLOWED_TOPICS_CONFIG");

  std::string config_path;

  if (env_path != nullptr)
  {
    config_path = env_path;
  }
  else
  {
    config_path = "ros_allowed_topics.conf";
  }

  std::ifstream ifs(config_path);

  if (!ifs.good())
  {
    printf("CRITICAL: configuration file '%s' could not be opened\n", config_path.c_str());
  }

  json config = json::parse(ifs);

  // Find object and convert to map
  std::map<std::string, uint8_t> topics = config.at("topics").get<std::map<std::string, uint8_t>>();

  std::map<uint8_t, std::string> identifiers;

  for (std::map<std::string, uint8_t>::iterator i = topics.begin(); i != topics.end(); ++i)
    identifiers[i->second] = i->first;

  _topics_list = topics;
  _identifiers_list = identifiers;
}

uint8_t TopicsConfig::get_topic_identifier(std::string name)
{
  if (_topics_list.find(name) == _topics_list.end())
    return 0;
  else
    return _topics_list.find(name)->second;
}

std::string TopicsConfig::get_identifier_topic(uint8_t identifier)
{
  if (_identifiers_list.find(identifier) == _identifiers_list.end())
    return "";
  else
    return _identifiers_list.find(identifier)->second;
}
