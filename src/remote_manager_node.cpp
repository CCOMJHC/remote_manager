// Roland Arsenault
// Center for Coastal and Ocean Mapping
// University of New Hampshire
// Copyright 2021, All rights reserved.

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "project11_msgs/Helm.h"
#include "udp_bridge/Subscribe.h"

std::string current_remote;

std::map<std::string, ros::Publisher> local_publishers;

struct Remote
{
  typedef std::map<std::string, ros::Publisher> PublisherMap;
  typedef std::map<std::string, ros::Subscriber> SubscriberMap;

  template<typename MessageType> void addPublisher(const std::string &topic)
  {
    ros::NodeHandle nh;

    std::string remote_topic = "/"+remote+"/"+topic;

    publishers[topic] = nh.advertise<MessageType>(remote_topic,1);

    ros::ServiceClient client = nh.serviceClient<udp_bridge::Subscribe>("udp_bridge/remote_advertise");
    udp_bridge::Subscribe sub;
    sub.request.remote = remote;
    sub.request.connection_id = "default";
    sub.request.destination_topic = remote_topic;
    sub.request.source_topic = remote_topic;
    sub.request.queue_size = 1;
    if(!client.waitForExistence(ros::Duration(5)))
      ROS_WARN_STREAM("Timeout waiting for udp_bridge remote advertise service");
    client.call(sub);
  }

  template<typename MessageType> void addSubscriber(const std::string &topic)
  {
    ros::NodeHandle nh;
    std::string remote_topic = "/"+remote+"/"+topic;
    subscribers[topic] = nh.subscribe<MessageType>(remote_topic,1,[&, topic](const typename MessageType::ConstPtr &msg){ if(remote == current_remote) local_publishers[topic].publish(msg);});
  }

  std::string remote;
  PublisherMap publishers;
  SubscriberMap subscribers;


};

typedef std::map<std::string, Remote> RemoteMap;
RemoteMap remotes;

void localHelmCallback(const project11_msgs::Helm::ConstPtr &msg)
{
  if(remotes.count(current_remote) && remotes[current_remote].publishers.count("piloting_mode/manual/helm"))
    remotes[current_remote].publishers["piloting_mode/manual/helm"].publish(msg);
}

void localCommandCallback(const std_msgs::String::ConstPtr &msg)
{
  if(remotes.count(current_remote) && remotes[current_remote].publishers.count("project11/command"))
    remotes[current_remote].publishers["project11/command"].publish(msg);
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "remote_manager");

  current_remote = ros::param::param<std::string>("~remote", "robot");

  ros::NodeHandle nh;

  ros::Subscriber local_helm_sub = nh.subscribe("project11/piloting_mode/manual/helm", 1, localHelmCallback);
  ros::Subscriber local_command_sub = nh.subscribe("project11/command", 1, localCommandCallback);

  local_publishers["project11/response"] = nh.advertise<std_msgs::String>("project11/response",1);


  remotes[current_remote].remote = current_remote;
  remotes[current_remote].addPublisher<project11_msgs::Helm>("project11/piloting_mode/manual/helm");
  remotes[current_remote].addPublisher<std_msgs::String>("project11/command");
  remotes[current_remote].addSubscriber<std_msgs::String>("project11/response");

  ros::spin();
    
  return 0;
}
