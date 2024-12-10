#include "demangle.h"


char * Discovery::integer_to_string(int x)
{
    char * buffer = static_cast<char *>(malloc(sizeof(char) * sizeof(int) * 4 + 1));
    if (buffer)
    {
         sprintf(buffer, "%d", x);
    }
    return buffer;
}

std::string Discovery::resolve_prefix(const std::string & name, const std::string & prefix)
{
  if (name.rfind(prefix, 0) == 0 && name.at(prefix.length()) == '/') {
    return name.substr(prefix.length());
  }
  return "";
}

std::string Discovery::demangle_publisher_from_topic(const std::string & topic_name)
{
  return resolve_prefix(topic_name, ros_topic_publisher_prefix);
}

std::string Discovery::demangle_subscriber_from_topic(const std::string & topic_name)
{
  return resolve_prefix(topic_name, ros_topic_subscriber_prefix);
}

std::string Discovery::demangle_topic(const std::string & topic_name)
{
  const std::string demangled_topic = demangle_publisher_from_topic(topic_name);
  if ("" != demangled_topic) {
    return demangled_topic;
  }
  return demangle_subscriber_from_topic(topic_name);
}

std::string Discovery::demangle_service_request_from_topic(const std::string & topic_name)
{
  return resolve_prefix(topic_name, ros_service_requester_prefix);
}

std::string Discovery::demangle_service_reply_from_topic(const std::string & topic_name)
{
  return resolve_prefix(topic_name, ros_service_response_prefix);
}

std::string Discovery::demangle_service_from_topic(const std::string & topic_name)
{
  const std::string demangled_topic = demangle_service_reply_from_topic(topic_name);
  if ("" != demangled_topic) {
    return demangled_topic;
  }
  return demangle_service_request_from_topic(topic_name);
}

std::string Discovery::identity_demangle(const std::string & name)
{
  return name;
}
