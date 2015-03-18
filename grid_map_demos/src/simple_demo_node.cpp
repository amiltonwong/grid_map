/*
 * simple_demo_node.cpp
 *
 *  Created on: Mar 17, 2014
 *      Author: Péter Fankhauser
 *   Institute: ETH Zurich, Autonomous Systems Lab
 */

#include <ros/ros.h>
#include <grid_map/grid_map.hpp>
#include <grid_map_msgs/GridMap.h>
#include <vector>
#include <string>
#include <cmath>

using namespace std;
using namespace grid_map_core;
using namespace grid_map;

int main(int argc, char** argv)
{
  ros::init(argc, argv, "grid_map_simple_demo");
  ros::NodeHandle nodeHandle("~");

  // Create grid map.
  vector<string> layerNames;
  layerNames.push_back("height");
  layerNames.push_back("std");
  grid_map::GridMap map(layerNames);
  map.setFrameId("map");
  map.setGeometry(Length(1.2, 2.0), 0.06);
  ROS_INFO("Created map with size %f x %f m (%i x %i cells).", map.getLength().x(),
           map.getLength().y(), map.getSize()(0), map.getSize()(1));

  // Add data.
  for (GridMapIterator iterator(map); !iterator.isPassedEnd(); ++iterator) {
    Position2 position;
    map.getPosition(*iterator, position);
    map.at("height", *iterator) = 0.2 * sin(5.0 * position.y()) * position.x();
  }

  map.get("std").setRandom();
  map.get("std") *= 0.01;

  // Manipulate data.
  Eigen::MatrixXf::Index maxRow, maxCol;
  float max = m.maxCoeff(&maxRow, &maxCol);



  // Publish grid map.
  map.setTimestamp(ros::Time::now().toNSec());
  grid_map_msgs::GridMap message;
  map.toMessage(message);
  ros::Publisher publisher = nodeHandle.advertise<grid_map_msgs::GridMap>("grid_map", 1, true);
  publisher.publish(message);
  ROS_INFO("Grid map (timestamp %f) published.", message.info.header.stamp.toSec());


  ros::waitForShutdown();
  return 0;
}