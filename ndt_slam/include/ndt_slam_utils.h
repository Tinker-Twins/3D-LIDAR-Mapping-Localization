#ifndef _NDT_SLAM_UTILS_
#define _NDT_SLAM_UTILS_

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <geometry_msgs/Pose.h>
#include <geometry_msgs/TransformStamped.h>

#include <tf2/LinearMath/Quaternion.h>
#include <tf2/convert.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <tf2_ros/static_transform_broadcaster.h>
#include <tf2_ros/transform_broadcaster.h>

#include <data_struct.h>

namespace ndt_slam_utils
{
double diffRadian(const double radian_a, const double radian_b)
{
  double diff_radian = radian_a - radian_b;
  if (M_PI <= diff_radian)
    diff_radian -= 2 * M_PI;
  else if (diff_radian < -M_PI)
    diff_radian += 2 * M_PI;
  return diff_radian;
}

geometry_msgs::Pose convertToGeometryPose(const Pose input_pose)
{
  geometry_msgs::Pose output_pose;
  output_pose.position.x = input_pose.x;
  output_pose.position.y = input_pose.y;
  output_pose.position.z = input_pose.z;
  tf2::Quaternion quat;
  quat.setRPY(input_pose.roll, input_pose.pitch, input_pose.yaw);
  output_pose.orientation.w = quat.w();
  output_pose.orientation.x = quat.x();
  output_pose.orientation.y = quat.y();
  output_pose.orientation.z = quat.z();
  return output_pose;
}

tf2::Transform convertToTransform(const Pose input_pose)
{
  tf2::Transform transform;
  transform.setOrigin(tf2::Vector3(input_pose.x, input_pose.y, input_pose.z));
  tf2::Quaternion quaternion;
  quaternion.setRPY(input_pose.roll, input_pose.pitch, input_pose.yaw);
  transform.setRotation(quaternion);

  return transform;
}

geometry_msgs::Pose convertGeometryTransformToGeometryPose(const geometry_msgs::TransformStamped transform_stamped)
{
  geometry_msgs::Pose pose;
  pose.position.x = transform_stamped.transform.translation.x;
  pose.position.y = transform_stamped.transform.translation.y;
  pose.position.z = transform_stamped.transform.translation.z;
  pose.orientation.w = transform_stamped.transform.rotation.w;
  pose.orientation.x = transform_stamped.transform.rotation.x;
  pose.orientation.y = transform_stamped.transform.rotation.y;
  pose.orientation.z = transform_stamped.transform.rotation.z;
  return pose;
}

Eigen::Matrix4f convertGeometryPoseToMatrix(const geometry_msgs::Pose pose)
{
  Eigen::Affine3d affine;
  tf2::fromMsg(pose, affine);
  Eigen::Matrix4f matrix = affine.matrix().cast<float>();
  return matrix;
}

Eigen::Matrix4f convertGeometryTransformToMatrix(const geometry_msgs::TransformStamped transform_stamped)
{
  return convertGeometryPoseToMatrix(convertGeometryTransformToGeometryPose(transform_stamped));
}

Eigen::Matrix4f convertPoseVecToMatrix(const Pose pose)
{
  return convertGeometryPoseToMatrix(convertToGeometryPose(pose));
}

Pose convertMatrixToPoseVec(const Eigen::Matrix4f pose)
{
  Pose vec;

  vec.x = pose(0, 3);
  vec.y = pose(1, 3);
  vec.z = pose(2, 3);

  tf2::Matrix3x3 mat;
  mat.setValue(
    pose(0, 0), pose(0, 1), pose(0, 2), pose(1, 0), pose(1, 1), pose(1, 2), pose(2, 0), pose(2, 1), pose(2, 2));
  mat.getRPY(vec.roll, vec.pitch, vec.yaw);

  return vec;
}

Pose convertQuaternionToPoseVec(const geometry_msgs::Quaternion quaternion)
{
  Pose pose;
  tf2::Quaternion quat_tf2(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
  tf2::Matrix3x3 mat(quat_tf2);
  mat.getRPY(pose.roll, pose.pitch, pose.yaw);
  return pose;
}

void publishTF(
  tf2_ros::TransformBroadcaster broadcaster, const Pose pose, const ros::Time stamp, const std::string frame_id,
  const std::string child_frame_id)
{
  geometry_msgs::Transform transform;
  geometry_msgs::TransformStamped transform_stamped;

  transform_stamped = tf2::toMsg(tf2::Stamped<tf2::Transform>(convertToTransform(pose), stamp, frame_id));
  transform_stamped.child_frame_id = child_frame_id;

  broadcaster.sendTransform(transform_stamped);
}

Eigen::VectorXd convertPoseToEigen(const Pose pose)
{
  Eigen::VectorXd vec(6);
  vec << pose.x, pose.y, pose.z, pose.roll, pose.pitch, pose.yaw;
  return vec;
}

Pose convertEigenToPose(const Eigen::VectorXd vec)
{
  Pose pose(vec(0), vec(1), vec(2), vec(3), vec(4), vec(5));
  return pose;
}

}  // namespace ndt_slam_utils

#endif
