/*
 * Circleterator.hpp
 *
 *  Created on: Nov 13, 2014
 *      Author: Péter Fankhauser
 *   Institute: ETH Zurich, Autonomous Systems Lab
 */

#include "grid_map_lib/iterators/CircleIterator.hpp"
#include "grid_map_lib/GridMapMath.hpp"

using namespace std;

namespace grid_map_lib {

CircleIterator::CircleIterator(const grid_map_lib::GridMap& gridMap, const Eigen::Vector2d& center,
                               const double radius)
    : center_(center),
      radius_(radius)
{
  radiusSquare_ = radius_ * radius_;
  mapLength_ = gridMap.getLength();
  mapPosition_ = gridMap.getPosition();
  resolution_ = gridMap.getResolution();
  bufferSize_ = gridMap.getBufferSize();
  bufferStartIndex_ = gridMap.getBufferStartIndex();
  Eigen::Array2i submapStartIndex;
  Eigen::Array2i submapBufferSize;
  findSubmapParameters(center, radius, submapStartIndex, submapBufferSize);
  internalIterator_ = std::shared_ptr<SubmapIterator>(new SubmapIterator(gridMap, submapStartIndex, submapBufferSize));
  if(!isInside()) ++(*this);
}

CircleIterator& CircleIterator::operator =(const CircleIterator& other)
{
  center_ = other.center_;
  radius_ = other.radius_;
  radiusSquare_ = other.radiusSquare_;
  internalIterator_ = other.internalIterator_;
  mapLength_ = other.mapLength_;
  mapPosition_ = other.mapPosition_;
  resolution_ = other.resolution_;
  bufferSize_ = other.bufferSize_;
  bufferStartIndex_ = other.bufferStartIndex_;
  return *this;
}

bool CircleIterator::operator !=(const CircleIterator& other) const
{
  return (internalIterator_ != other.internalIterator_);
}

const Eigen::Array2i& CircleIterator::operator *() const
{
  return *(*internalIterator_);
}

CircleIterator& CircleIterator::operator ++()
{
  ++(*internalIterator_);
  if (internalIterator_->isPassedEnd()) return *this;

  for ( ; !internalIterator_->isPassedEnd(); ++(*internalIterator_)) {
    if (isInside()) break;
  }

  return *this;
}

bool CircleIterator::isPassedEnd() const
{
  return internalIterator_->isPassedEnd();
}

bool CircleIterator::isInside()
{
  Eigen::Vector2d position;
  getPositionFromIndex(position, *(*internalIterator_), mapLength_, mapPosition_, resolution_, bufferSize_, bufferStartIndex_);
  double squareNorm = (position - center_).array().square().sum();
  return (squareNorm <= radiusSquare_);
}

void CircleIterator::findSubmapParameters(const Eigen::Vector2d& center, const double radius,
                                          Eigen::Array2i& startIndex,
                                          Eigen::Array2i& bufferSize) const
{
  Eigen::Vector2d topLeft = center.array() + radius;
  Eigen::Vector2d bottomRight = center.array() - radius;
  limitPositionToRange(topLeft, mapLength_, mapPosition_);
  limitPositionToRange(bottomRight, mapLength_, mapPosition_);
  getIndexFromPosition(startIndex, topLeft, mapLength_, mapPosition_, resolution_, bufferSize_, bufferStartIndex_);
  Eigen::Array2i endIndex;
  getIndexFromPosition(endIndex, bottomRight, mapLength_, mapPosition_, resolution_, bufferSize_, bufferStartIndex_);
  bufferSize = endIndex - startIndex + Eigen::Array2i::Ones();
}

} /* namespace grid_map_lib */

