#pragma once

#include <time_series/time_series.hpp>

#include <algorithm>
#include <fstream>
#include <math.h>
#include <mutex>
#include <optional>
#include <tuple>
#include <unordered_map>

#include "monopod_sdk/common_header.hpp"
#include "monopod_sdk/monopod_drivers/leg.hpp"

namespace monopod_drivers {

/**
 * @brief Drivers for open sim2real monopod. Interfaces with the monopod TI
 * motors using monopod_drivers::BlmcJointModule. This class creates a real time
 * control thread which reads and writes from a buffer exposed to the public
 * api.
 */
class Monopod {

public:
  struct PID;
  struct JointLimit;

  /**
   * @brief Construct a new Monopod object.
   *
   */
  Monopod();

  /**
   * @brief Destroy the Monopod object
   */
  ~Monopod();

  /**
   * @brief Initialize can_bus connections to encoder board and motor board.
   *
   * @param monopod_mode defines the task mode of the monopod. Can also specify
   * individual boards.
   */
  bool initialize(Mode monopod_mode);

  /**
   * @brief is the monopod sdk Initialized?.
   */
  bool initialized();

  /**
   * @brief Calibrate the Encoders.
   *
   * @param hip_home_offset_rad hip offset from found encoder index 0 (rad)
   * @param knee_home_offset_rad knee offset from found encoder index 0 (rad)
   */
  void calibrate(const double &hip_home_offset_rad = 0,
                 const double &knee_home_offset_rad = 0);

  /**
   * @brief Get model name
   *
   * @return String of model name
   */
  std::string get_model_name() const;

  /**
   * @brief Get a map of 'active' joint strings indexing their enumerator index
   *
   * @return Unordered map of joint name strings as key and index as value
   */
  std::unordered_map<std::string, int> get_joint_names() const;

  /**
   * @brief check if the joint is a controllable joint (has a motor) or only
   * a observation joint (encoder only).
   *
   * @param joint_index name of the joint we want to access
   * @return bool whether joint is controllable
   */
  bool is_joint_controllable(const int joint_index);

  // ======================================
  // setters
  //=======================================

  /**
   * @brief Set the torque target for some joint index. Return a bool whether
   * successful.
   *
   * @param torque_target is the desired torque target for indexed joint
   * @param joint_index name of the joint we want to access
   * @return bool whether setting the value was successfull
   */
  bool set_torque_target(const double &torque_target, const int joint_index);

  /**
   * @brief Set the torque targets for all joint indexes. Return a bool whether
   * successful.
   *
   * @param torque_targets vector of desired torque targets for indexed joints
   * @param joint_indexes names of the joints we want to access
   * @return bool whether setting the value was successfull
   */
  bool set_torque_targets(const std::vector<double> &torque_targets,
                          const std::vector<int> &joint_indexes = {});

  /**
   * Set the PID parameters of the joint.
   *
   * @param pid The desired PID parameters.
   * @return True for success, false otherwise.
   */
  bool set_pid(const int &p, const int &i, const int &d,
               const int &joint_index);

  /**
   * Set the maximum Position of the joint.
   *
   * This limit when reached will kill the robot for safety
   *
   * @param max A double with the maximum position of the joint.
   * @param min A double with the minimum position of the joint.
   * @param joint_index name of the joint we want to access
   * @return True for success, false otherwise.
   */
  bool set_joint_position_limit(const double &max, const double &min,
                                const int &joint_index);

  /**
   * Set the maximum velocity of the joint.
   *
   * This limit when reached will kill the robot for safety
   *
   * @param max A double with the maximum velocity of the joint.
   * @param min A double with the minimum velocity of the joint.
   * @param joint_index name of the joint we want to access
   * @return True for success, false otherwise.
   */
  bool set_joint_velocity_limit(const double &max, const double &min,
                                const int &joint_index);

  /**
   * Set the maximum acceleration of the joint.
   *
   * This limit when reached will kill the robot for safety
   *
   * @param max A double with the maximum acceleration of the joint.
   * @param min A double with the minimum acceleration of the joint.
   * @param joint_index name of the joint we want to access
   * @return True for success, false otherwise.
   */
  bool set_joint_acceleration_limit(const double &max, const double &min,
                                    const int &joint_index);

  /**
   * Set the maximum torque target of the joint.
   *
   * This limit when reached will kill the robot for safety
   *
   * @param max_torque_target A double with the maximum torque of the joint.
   * @param joint_index name of the joint we want to access
   * @return True for success, false otherwise.
   */
  bool set_max_torque_target(const double &max_torque_target,
                             const int &joint_index);

  // ======================================
  // getters
  //=======================================

  /**
   * Get the PID parameters of the joint.
   *
   * If no PID parameters have been set, the default parameters are
   * returned.
   *
   * @return The joint PID parameters.
   */
  std::optional<PID> get_pid(const int &joint_index);

  /**
   * Get the position limits of the joint.
   *
   * @return The position limits of the joint.
   */
  std::optional<JointLimit> get_joint_position_limit(const int &joint_index);

  /**
   * Get the velocity limits of the joint.
   *
   * @return The velocity limits of the joint.
   */
  std::optional<JointLimit> get_joint_velocity_limit(const int &joint_index);

  /**
   * Get the velocity limits of the joint.
   *
   * @return The velocity limits of the joint.
   */
  std::optional<JointLimit>
  get_joint_acceleration_limit(const int &joint_index);

  /**
   * @brief Get the max torque
   *
   * @param joint_index
   * @return std::optional<double> containing the max torque if success
   */
  std::optional<double> get_max_torque_target(const int &joint_index);

  /**
   * @brief Get the torque
   *
   * @param joint_index
   * @return std::optional<double> containing the torque if success
   */
  std::optional<double> get_torque_target(const int &joint_index);

  /**
   * @brief Get the torques of indexed joints
   *
   * @param joint_index
   * @return std::optional<double> containing the torque if success
   */
  std::optional<std::vector<double>>
  get_torque_targets(const std::vector<int> &joint_indexes = {});

  /**
   * @brief Get the position of joint
   *
   * @param joint_index name of the joint we want to access
   * @return std::optional<double> containing the position if success
   * value of the position (NULL if not valid)
   */
  std::optional<double> get_position(const int &joint_index);

  /**
   * @brief Get the velocity of the joint
   *
   * @param joint_index name of the joint we want to access
   * @return std::optional<double> containing the velocity if success
   */
  std::optional<double> get_velocity(const int &joint_index);

  /**
   * @brief Get the acceleration of the joint
   *
   * @param joint_index name of the joint we want to access
   * @return std::optional<double> containing the acceleration if success
   */
  std::optional<double> get_acceleration(const int &joint_index);

  /**
   * @brief Get the position of the joint indexes
   *
   * @param joint_indexes names of the joints we want to access
   * @return std::optional<vector<double>> containing vector of positions if
   * success
   */
  std::optional<std::vector<double>>
  get_positions(const std::vector<int> &joint_indexes = {});

  /**
   * @brief Get the velocity of the joint indexes
   *
   * @param joint_indexes names of the joints we want to access
   * @return std::optional<std::vector<double>> containing vector of velocities
   * if success
   */
  std::optional<std::vector<double>>
  get_velocities(const std::vector<int> &joint_indexes = {});

  /**
   * @brief Get the acceleration of the joint indexes
   *
   * @param joint_indexes names of the joints we want to access
   * @return std::optional<std::vector<double>> containing vector of
   * accelerations if success
   */
  std::optional<std::vector<double>>
  get_accelerations(const std::vector<int> &joint_indexes = {});

private:
  /**
   * @brief Simple helper method to serialized getting of data.
   *
   * Gets indees on joint index enum
   */

  std::optional<std::vector<double>>
  getJointDataSerialized(const Monopod *monopod,
                         const std::vector<int> &joint_indexes,
                         std::function<double(int)> getJointData) {
    // Take the joint index in lambda. Return the data you want.
    const std::vector<int> &jointSerialization =
        joint_indexes.empty() ? monopod->read_joint_indexing : joint_indexes;

    std::vector<double> data;
    data.reserve(jointSerialization.size());
    for (auto &joint_index : jointSerialization) {
      if (is_initialized && Contains(read_joint_indexing, joint_index)) {
        data.push_back(getJointData(joint_index));
      } else {
        return std::nullopt;
      }
    }
    return data;
  }

  std::shared_ptr<EncoderJointModule>
  create_encoder_module(JointNameIndexing joint_index) {
    /* Create encoders here */
    auto encoder =
        std::make_shared<monopod_drivers::Encoder>(can_bus_board_, joint_index);
    /* Encoder joint modules */
    return std::make_shared<EncoderJointModule>(joint_index, encoder, 1.0, 0.0,
                                                false);
  }

  std::shared_ptr<MotorJointModule>
  create_motor_module(JointNameIndexing joint_index) {
    /* create motors here*/
    auto motor =
        std::make_shared<monopod_drivers::Motor>(can_bus_board_, joint_index);
    /* motor joint modules */
    return std::make_shared<MotorJointModule>(joint_index, motor, 0.025, 9.0,
                                              0.0, false);
  }

  /**
   * @brief Template helper for getting sign
   */
  template <typename T> static int sgn(T val) {
    return (T(0) < val) - (val < T(0));
  }

  /**
   * @brief Template helper checking if vector contains an element.
   */
  template <typename T>
  bool Contains(const std::vector<T> &Vec, const T &Element) const {
    if (std::find(Vec.begin(), Vec.end(), Element) != Vec.end())
      return true;
    return false;
  }

  /**
   * @brief Template helper checking if vector contains an element.
   */
  template <typename T> static bool in_range(T value, T min, T max) {
    return min <= value && value < max;
  }

public:
  /**
   * @brief Joint names indexed same as enumerator
   */
  const std::unordered_map<std::string, int> joint_names = {
      {"hip_joint", hip_joint},
      {"knee_joint", knee_joint},
      {"boom_connector_joint", boom_connector_joint},
      {"planarizer_yaw_joint", planarizer_yaw_joint},
      {"planarizer_pitch_joint", planarizer_pitch_joint}};

  /**
   * @brief Structure holding the observed state of a joint
   */
  struct PID {
    PID() = default;
    PID(const double _p, const double _i, const double _d)
        : p(_p), i(_i), d(_d) {}

    double p = 0;
    double i = 0;
    double d = 0;
  };

  /**
   * @brief Structure holding joint limits
   */
  struct JointLimit {
    JointLimit() {
      constexpr double m = std::numeric_limits<double>::lowest();
      constexpr double M = std::numeric_limits<double>::max();

      min = m;
      max = M;
    }

    JointLimit(const double _min, const double _max) : min(_min), max(_max) {}

    double min;
    double max;
  };

private:
  /**
   * @brief Canbus connection.
   */
  std::shared_ptr<monopod_drivers::CanBus> can_bus_;

  /**
   * @brief Canbus ControlBoards. This maintains connection with the canbus and
   * holds meassurement and write buffers.
   */
  std::shared_ptr<monopod_drivers::CanBusControlBoards> can_bus_board_;

  /**
   * @brief Holds encoder joint modules for each active joint.
   */
  std::unordered_map<int, std::shared_ptr<EncoderJointModule>> encoders_;

  /**
   * @brief Holds motor joint modules for each active controllable joint.
   */
  std::unordered_map<int, std::shared_ptr<MotorJointModule>> motors_;

  /**
   * @brief The task mode of the monopod. Either predefined or custom.
   */
  Mode monopod_mode_;

  /**
   * @brief robot Leg interface object. This is used for calibration and coupled
   * actions like goto position. I m not sure if this is how we should handle
   * it.
   */
  std::unique_ptr<monopod_drivers::Leg> leg_;

  /**
   * @brief boolen defining if sdk is initialized.
   */
  bool is_initialized;

  /**
   * @brief Write Joint names indexed same as enumerator for actuators.  All
   * valid controlled joints should be defined here.
   */
  std::vector<int> write_joint_indexing;

  /**
   * @brief Read Joint names indexed same as enumerator for encoders. All valid
   * joints should be defined here.
   */
  std::vector<int> read_joint_indexing;

  /**
   * @brief Structure holding the observed state of a joint
   */
  struct JointSettingState {
    JointSettingState() = default;
    JointSettingState(const double _max_torque_target,
                      const JointLimit _position_limit,
                      const JointLimit _velocity_limit,
                      const JointLimit _acceleration_limit)
        : position_limit(_position_limit), velocity_limit(_velocity_limit),
          acceleration_limit(_acceleration_limit),
          max_torque_target(_max_torque_target) {}

    JointLimit position_limit = {};
    JointLimit velocity_limit = {};
    JointLimit acceleration_limit = {};
    double max_torque_target = 0;
  };

}; // end class Monopod definition

} // namespace monopod_drivers
