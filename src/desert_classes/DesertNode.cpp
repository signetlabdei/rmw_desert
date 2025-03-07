#include "DesertNode.h"

DesertNode::DesertNode(std::string name, std::string namespace_, rmw_gid_t gid)
      : _gid(gid)
      , _name(name)
      , _namespace(namespace_)
      , _discovery_beacon_data_stream(cbor::TxStream(PUBLISHER_TYPE, "discovery", TopicsConfig::get_topic_identifier("/discovery")))
      , _discovery_request_data_stream(cbor::RxStream(SUBSCRIBER_TYPE, "discovery_request", TopicsConfig::get_topic_identifier("/discovery_request")))
{
  if (!TopicsConfig::get_topic_identifier("/discovery_request"))
    return;
  
  _discovery_done = false;
  _discovery_request_thread = std::thread(&DesertNode::_discovery_request, this);
}

DesertNode::~DesertNode()
{
  if (!TopicsConfig::get_topic_identifier("/discovery_request"))
    return;
  
  _discovery_done = true;
  _discovery_request_thread.join();
}

void DesertNode::add_publisher(DesertPublisher * pub)
{
  if (!TopicsConfig::get_topic_identifier(pub->get_topic_name()))
    return;
  
  _publishers.push_back(pub);

  if (!TopicsConfig::get_topic_identifier("/discovery"))
    return;

  Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, PUBLISHER_TYPE, pub->get_gid() , pub->get_topic_name(), pub->get_type_name(), false);
}

void DesertNode::add_subscriber(DesertSubscriber * sub)
{
  if (!TopicsConfig::get_topic_identifier(sub->get_topic_name()))
    return;
  
  _subscribers.push_back(sub);
  
  if (!TopicsConfig::get_topic_identifier("/discovery"))
    return;
  
  Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, SUBSCRIBER_TYPE, sub->get_gid() , sub->get_topic_name(), sub->get_type_name(), false);
}

void DesertNode::add_client(DesertClient * cli)
{
  if (!TopicsConfig::get_topic_identifier(cli->get_service_name()))
    return;
  
  _clients.push_back(cli);
  
  if (!TopicsConfig::get_topic_identifier("/discovery"))
    return;
  
  Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, CLIENT_TYPE, cli->get_gid() , cli->get_service_name(), cli->get_request_type_name(), false);
}

void DesertNode::add_service(DesertService * ser)
{
  if (!TopicsConfig::get_topic_identifier(ser->get_service_name()))
    return;
  
  _services.push_back(ser);
  
  if (!TopicsConfig::get_topic_identifier("/discovery"))
    return;
  
  Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, SERVICE_TYPE, ser->get_gid() , ser->get_service_name(), ser->get_response_type_name(), false);
}

void DesertNode::remove_publisher(DesertPublisher * pub)
{
  if (!TopicsConfig::get_topic_identifier(pub->get_topic_name()))
    return;

  if (!TopicsConfig::get_topic_identifier("/discovery"))
    return;
  
  Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, PUBLISHER_TYPE, pub->get_gid() , "", "", true);
  
  std::vector<DesertPublisher *>::iterator position = std::find(_publishers.begin(), _publishers.end(), pub);
  
  if (position != _publishers.end())
      _publishers.erase(position);
}

void DesertNode::remove_subscriber(DesertSubscriber * sub)
{
  if (!TopicsConfig::get_topic_identifier(sub->get_topic_name()))
    return;

  if (!TopicsConfig::get_topic_identifier("/discovery"))
    return;
  
  Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, SUBSCRIBER_TYPE, sub->get_gid() , "", "", true);
  
  std::vector<DesertSubscriber *>::iterator position = std::find(_subscribers.begin(), _subscribers.end(), sub);
  
  if (position != _subscribers.end())
      _subscribers.erase(position);
}

void DesertNode::remove_client(DesertClient * cli)
{
  if (!TopicsConfig::get_topic_identifier(cli->get_service_name()))
    return;

  if (!TopicsConfig::get_topic_identifier("/discovery"))
    return;
  
  Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, CLIENT_TYPE, cli->get_gid() , "", "", true);
  
  std::vector<DesertClient *>::iterator position = std::find(_clients.begin(), _clients.end(), cli);
  
  if (position != _clients.end())
      _clients.erase(position);
}

void DesertNode::remove_service(DesertService * ser)
{
  if (!TopicsConfig::get_topic_identifier(ser->get_service_name()))
    return;

  if (!TopicsConfig::get_topic_identifier("/discovery"))
    return;
  
  Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, SERVICE_TYPE, ser->get_gid() , "", "", true);
  
  std::vector<DesertService *>::iterator position = std::find(_services.begin(), _services.end(), ser);
  
  if (position != _services.end())
      _services.erase(position);
}

rmw_gid_t DesertNode::get_gid()
{
  return _gid;
}

void DesertNode::publish_all_beacons()
{
  for (DesertPublisher * pub : _publishers)
    Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, PUBLISHER_TYPE, pub->get_gid() , pub->get_topic_name(), pub->get_type_name(), false);
  
  for (DesertSubscriber * sub : _subscribers)
    Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, SUBSCRIBER_TYPE, sub->get_gid() , sub->get_topic_name(), sub->get_type_name(), false);
  
  for (DesertClient * cli : _clients)
    Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, CLIENT_TYPE, cli->get_gid() , cli->get_service_name(), cli->get_request_type_name(), false);
  
  for (DesertService * ser : _services)
    Discovery::send_discovery_beacon(_discovery_beacon_data_stream, _name, _namespace, SERVICE_TYPE, ser->get_gid() , ser->get_service_name(), ser->get_response_type_name(), false);
}

void DesertNode::_discovery_request()
{
  while (!_discovery_done)
  {
    cbor::RxStream::interpret_packets();
    if (_discovery_request_data_stream.data_available())
    {
      publish_all_beacons();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}
