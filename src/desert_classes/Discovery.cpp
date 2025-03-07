#include "Discovery.h"

void Discovery::discovery_thread(rmw_context_impl_t * impl)
{
  cbor::RxStream discovery_beacon_stream = cbor::RxStream(SUBSCRIBER_TYPE, "/discovery", TopicsConfig::get_topic_identifier("/discovery"));
  cbor::TxStream discovery_request_stream = cbor::TxStream(PUBLISHER_TYPE, "/discovery_request", TopicsConfig::get_topic_identifier("/discovery_request"));
  
  send_discovery_request(discovery_request_stream);
  
  while (true)
  {
    cbor::RxStream::interpret_packets();
    if (discovery_beacon_stream.data_available())
    {
      rmw_dds_common::msg::ParticipantEntitiesInfo msg;
      rmw_dds_common::msg::NodeEntitiesInfo node;
      rmw_dds_common::msg::Gid gid;
      
      int entity_type;
      std::vector<uint8_t> entity_gid;
      std::string topic_name, type_name;
      bool disconnect;
      
      discovery_beacon_stream >> node.node_name;
      discovery_beacon_stream >> node.node_namespace;
      discovery_beacon_stream >> entity_type;
      discovery_beacon_stream >> entity_gid;
      discovery_beacon_stream >> topic_name;
      discovery_beacon_stream >> type_name;
      discovery_beacon_stream >> disconnect;
      
      discovery_beacon_stream.clear_buffer();
      
      
      std::copy_n(entity_gid.begin(), RMW_GID_STORAGE_SIZE, gid.data.begin());
      
      if (!disconnect)
      {
        if (entity_type == SUBSCRIBER_TYPE)
        {
          node.reader_gid_seq.push_back(gid);
        }
        else
        {
          node.writer_gid_seq.push_back(gid);
        }
      }
      
      rcutils_allocator_t allocator = rcutils_get_default_allocator();
      rmw_names_and_types_t * topic_names_and_types = new rmw_names_and_types_t();
      rmw_ret_t ret;
      (void) ret;
      
      // START Retreive cached writers
      ret = impl->common.graph_cache.get_writer_names_and_types_by_node(
        node.node_name,
        node.node_namespace,
        Discovery::identity_demangle,
        Discovery::identity_demangle,
        &allocator,
        topic_names_and_types);
      
      for (size_t i = 0; i < topic_names_and_types->names.size; i++)
      {
        rmw_topic_endpoint_info_array_t * endpoint_info_array = new rmw_topic_endpoint_info_array_t();
        const std::string mangled_topic_name = std::string(topic_names_and_types->names.data[i]);
        ret = impl->common.graph_cache.get_writers_info_by_topic(
          mangled_topic_name,
          Discovery::identity_demangle,
          &allocator,
          endpoint_info_array);

        for (size_t c = 0; c < endpoint_info_array->size; c++)
        {
          rmw_topic_endpoint_info_t endpoint_info = endpoint_info_array->info_array[c];
            
          // Check if matches the current node
          if (std::string(endpoint_info.node_name) != node.node_name || std::string(endpoint_info.node_namespace) != node.node_namespace)
            continue;
          
          rmw_dds_common::msg::Gid previous_gid;
          std::copy(std::begin(endpoint_info.endpoint_gid), std::end(endpoint_info.endpoint_gid), std::begin(previous_gid.data));
          
          // Check if the entity is disconnecting
          if (disconnect && gid.data == previous_gid.data)
            continue;
          
          node.writer_gid_seq.push_back(previous_gid);
        }
      }
      // END Retreive cached writers
      
      // START Retreive cached readers
      ret = impl->common.graph_cache.get_reader_names_and_types_by_node(
        node.node_name,
        node.node_namespace,
        Discovery::identity_demangle,
        Discovery::identity_demangle,
        &allocator,
        topic_names_and_types);
      
      for (size_t i = 0; i < topic_names_and_types->names.size; i++)
      {
        rmw_topic_endpoint_info_array_t * endpoint_info_array = new rmw_topic_endpoint_info_array_t();
        const std::string mangled_topic_name = std::string(topic_names_and_types->names.data[i]);
        ret = impl->common.graph_cache.get_readers_info_by_topic(
          mangled_topic_name,
          Discovery::identity_demangle,
          &allocator,
          endpoint_info_array);

        for (size_t c = 0; c < endpoint_info_array->size; c++)
        {
          rmw_topic_endpoint_info_t endpoint_info = endpoint_info_array->info_array[c];
            
          // Check if matches the current node
          if (std::string(endpoint_info.node_name) != node.node_name || std::string(endpoint_info.node_namespace) != node.node_namespace)
            continue;
          
          rmw_dds_common::msg::Gid previous_gid;
          std::copy(std::begin(endpoint_info.endpoint_gid), std::end(endpoint_info.endpoint_gid), std::begin(previous_gid.data));
          
          // Check if the entity is disconnecting
          if (disconnect && gid.data == previous_gid.data)
            continue;
          
          node.reader_gid_seq.push_back(previous_gid);
        }
      }
      // END Retreive cached readers
      
      msg.node_entities_info_seq.push_back(node);
    
      impl->common.graph_cache.update_participant_entities(msg);
      
      rmw_gid_t topic_gid = impl->common.gid;
      std::copy(std::begin(gid.data), std::end(gid.data), std::begin(topic_gid.data));
      
      if (disconnect)
      {
        impl->common.graph_cache.remove_entity(topic_gid, entity_type == SUBSCRIBER_TYPE);
      }
      else
      {
        impl->common.graph_cache.add_entity(
              topic_gid,
              std::to_string(entity_type) + topic_name,
              type_name,
              rosidl_get_zero_initialized_type_hash(),
              impl->common.gid,
              rmw_qos_profile_default,
              entity_type == SUBSCRIBER_TYPE);
      }
    
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}

rmw_ret_t Discovery::discovery_thread_start(rmw_context_impl_t * impl)
{
  auto common_context = &impl->common;
  
  if (!common_context->thread_is_running)
  {
    common_context->thread_is_running.store(true);
    
    try
    {
      common_context->listener_thread = std::thread(discovery_thread, impl);
      return RMW_RET_OK;
    }
    catch (const std::exception & exc)
    {
      RMW_SET_ERROR_MSG_WITH_FORMAT_STRING("Failed to create std::thread: %s", exc.what());
    }
    catch (...)
    {
      RMW_SET_ERROR_MSG("Failed to create std::thread");
    }
    
    return RMW_RET_ERROR;
  }
  
  return RMW_RET_OK;
}

void Discovery::send_discovery_beacon(cbor::TxStream stream, std::string node_name, std::string node_namespace, int entity_type, rmw_gid_t entity_gid, std::string topic_name, std::string type_name, bool disconnect)
{
  std::vector<uint8_t> gid_data(std::begin(entity_gid.data), std::end(entity_gid.data));
  
  stream.start_transmission();
  
  stream << node_name;
  stream << node_namespace;
  stream << entity_type;
  stream << gid_data;
  stream << topic_name;
  stream << type_name;
  stream << disconnect;
  
  stream.end_transmission();
}

void Discovery::send_discovery_request(cbor::TxStream stream)
{
  stream.start_transmission();
  
  stream.end_transmission();
}
