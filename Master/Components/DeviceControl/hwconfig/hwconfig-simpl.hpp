// Copyright (c) 2005-2011 Code Synthesis Tools CC
//
// This program was generated by CodeSynthesis XSD/e, an XML Schema
// to C++ data binding compiler for embedded systems.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
//

#ifndef HWCONFIG_SIMPL_HPP
#define HWCONFIG_SIMPL_HPP

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#ifndef XSDE_OMIT_SAGGR
#  define XSDE_OMIT_SAGGR
#  define HWCONFIG_SIMPL_HPP_CLEAR_OMIT_SAGGR
#endif

#include "hwconfig-sskel.hpp"

#include <xsde/cxx/stack.hxx>

class can_interfaceType_simpl: public can_interfaceType_sskel
{
  public:
  can_interfaceType_simpl ();

  virtual void
  pre (const ::can_interfaceType&);

  // Attributes.
  //
  virtual bool
  interface_present ();

  virtual ::std::string
  interface ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct can_interfaceType_simpl_state
  {
    const ::can_interfaceType* can_interfaceType_;
  };

  can_interfaceType_simpl_state can_interfaceType_simpl_state_;
};

class tcp_interfaceType_simpl: public tcp_interfaceType_sskel
{
  public:
  tcp_interfaceType_simpl ();

  virtual void
  pre (const ::tcp_interfaceType&);

  // Attributes.
  //
  virtual bool
  usage_present ();

  virtual signed char
  usage ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct tcp_interfaceType_simpl_state
  {
    const ::tcp_interfaceType* tcp_interfaceType_;
  };

  tcp_interfaceType_simpl_state tcp_interfaceType_simpl_state_;
};

class serial_numberType_simpl: public serial_numberType_sskel
{
  public:
  serial_numberType_simpl ();

  virtual void
  pre (const ::serial_numberType&);

  // Attributes.
  //
  virtual bool
  serialno_present ();

  virtual ::std::string
  serialno ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct serial_numberType_simpl_state
  {
    const ::serial_numberType* serial_numberType_;
  };

  serial_numberType_simpl_state serial_numberType_simpl_state_;
};

class deviceType_simpl: public deviceType_sskel
{
  public:
  virtual void
  pre (const ::deviceType&);

  // Attributes.
  //
  virtual bool
  name_present ();

  virtual ::std::string
  name ();

  virtual bool
  id_present ();

  virtual ::std::string
  id ();

  virtual bool
  type_present ();

  virtual ::std::string
  type ();

  virtual bool
  dev_instanceID_present ();

  virtual ::std::string
  dev_instanceID ();

  virtual bool
  optional_present ();

  virtual signed char
  optional ();

  // Elements.
  //
  virtual bool
  functionmodules_present ();

  virtual const ::functionmodulesType&
  functionmodules ();

  public:
  struct deviceType_simpl_state
  {
    const ::deviceType* deviceType_;
  };

  deviceType_simpl_state deviceType_simpl_state_;
};

class devicesType_simpl: public devicesType_sskel
{
  public:
  virtual void
  pre (const ::devicesType&);

  // Elements.
  //
  virtual bool
  device_next ();

  virtual const ::deviceType&
  device ();

  public:
  struct devicesType_simpl_state
  {
    const ::devicesType* devicesType_;
    ::devicesType::device_const_iterator device_;
    ::devicesType::device_const_iterator device_end_;
  };

  devicesType_simpl_state devicesType_simpl_state_;
};

class retortType_simpl: public retortType_sskel
{
  public:
  virtual void
  pre (const ::retortType&);

  // Attributes.
  //
  virtual bool
  name_present ();

  virtual ::std::string
  name ();

  virtual bool
  index_present ();

  virtual signed char
  index ();

  // Elements.
  //
  virtual const ::devicesType&
  devices ();

  public:
  struct retortType_simpl_state
  {
    const ::retortType* retortType_;
  };

  retortType_simpl_state retortType_simpl_state_;
};

class retortsType_simpl: public retortsType_sskel
{
  public:
  virtual void
  pre (const ::retortsType&);

  // Elements.
  //
  virtual bool
  retort_next ();

  virtual const ::retortType&
  retort ();

  public:
  struct retortsType_simpl_state
  {
    const ::retortsType* retortsType_;
    ::retortsType::retort_const_iterator retort_;
    ::retortsType::retort_const_iterator retort_end_;
  };

  retortsType_simpl_state retortsType_simpl_state_;
};

class parameter_masterType_simpl: public parameter_masterType_sskel
{
  public:
  virtual void
  pre (const ::parameter_masterType&);

  // Attributes.
  //
  virtual bool
  folded_present ();

  virtual ::std::string
  folded ();

  // Elements.
  //
  virtual const ::can_interfaceType&
  can_interface ();

  virtual const ::tcp_interfaceType&
  tcp_interface ();

  virtual signed char
  nodetype ();

  virtual signed char
  nodeindex ();

  virtual const ::serial_numberType&
  serial_number ();

  virtual const ::retortsType&
  retorts ();

  public:
  struct parameter_masterType_simpl_state
  {
    const ::parameter_masterType* parameter_masterType_;
  };

  parameter_masterType_simpl_state parameter_masterType_simpl_state_;
};

class rotationType_simpl: public rotationType_sskel
{
  public:
  rotationType_simpl ();

  virtual void
  pre (const ::rotationType&);

  // Attributes.
  //
  virtual bool
  type_present ();

  virtual ::std::string
  type ();

  virtual bool
  direction_present ();

  virtual ::std::string
  direction ();

  virtual bool
  steps_revolution_present ();

  virtual short
  steps_revolution ();

  virtual bool
  position_min_present ();

  virtual short
  position_min ();

  virtual bool
  position_max_present ();

  virtual short
  position_max ();

  virtual bool
  speed_min_present ();

  virtual signed char
  speed_min ();

  virtual bool
  speed_max_present ();

  virtual short
  speed_max ();

  virtual bool
  run_cs_present ();

  virtual signed char
  run_cs ();

  virtual bool
  stop_cs_present ();

  virtual signed char
  stop_cs ();

  virtual bool
  stop_cs_delay_present ();

  virtual short
  stop_cs_delay ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct rotationType_simpl_state
  {
    const ::rotationType* rotationType_;
  };

  rotationType_simpl_state rotationType_simpl_state_;
};

class reference_runType_simpl: public reference_runType_sskel
{
  public:
  reference_runType_simpl ();

  virtual void
  pre (const ::reference_runType&);

  // Attributes.
  //
  virtual bool
  ref_position_present ();

  virtual signed char
  ref_position ();

  virtual bool
  max_distance_present ();

  virtual short
  max_distance ();

  virtual bool
  timeout_present ();

  virtual int
  timeout ();

  virtual bool
  reverse_distance_present ();

  virtual signed char
  reverse_distance ();

  virtual bool
  slow_speed_present ();

  virtual signed char
  slow_speed ();

  virtual bool
  high_speed_present ();

  virtual short
  high_speed ();

  virtual bool
  refpos_offset_present ();

  virtual signed char
  refpos_offset ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct reference_runType_simpl_state
  {
    const ::reference_runType* reference_runType_;
  };

  reference_runType_simpl_state reference_runType_simpl_state_;
};

class encoderType_simpl: public encoderType_sskel
{
  public:
  encoderType_simpl ();

  virtual void
  pre (const ::encoderType&);

  // Attributes.
  //
  virtual bool
  type_present ();

  virtual signed char
  type ();

  virtual bool
  resolution_present ();

  virtual short
  resolution ();

  virtual bool
  rotation_present ();

  virtual ::std::string
  rotation ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct encoderType_simpl_state
  {
    const ::encoderType* encoderType_;
  };

  encoderType_simpl_state encoderType_simpl_state_;
};

class configurationType_simpl: public configurationType_sskel
{
  public:
  configurationType_simpl ();

  virtual void
  pre (const ::configurationType&);

  // Attributes.
  //
  virtual bool
  polarity_present ();

  virtual signed char
  polarity ();

  virtual bool
  sample_rate_present ();

  virtual signed char
  sample_rate ();

  virtual bool
  debounce_present ();

  virtual signed char
  debounce ();

  virtual bool
  temp_tolerance_present ();

  virtual signed char
  temp_tolerance ();

  virtual bool
  sampling_period_present ();

  virtual short
  sampling_period ();

  virtual bool
  fan_speed_present ();

  virtual short
  fan_speed ();

  virtual bool
  fan_threshold_present ();

  virtual short
  fan_threshold ();

  virtual bool
  current_gain_present ();

  virtual short
  current_gain ();

  virtual bool
  heater_current_present ();

  virtual short
  heater_current ();

  virtual bool
  heater_threshold_present ();

  virtual short
  heater_threshold ();

  virtual bool
  current_deviation_present ();

  virtual short
  current_deviation ();

  virtual bool
  current_min_230_serial_present ();

  virtual short
  current_min_230_serial ();

  virtual bool
  current_max_230_serial_present ();

  virtual short
  current_max_230_serial ();

  virtual bool
  current_min_100_serial_present ();

  virtual short
  current_min_100_serial ();

  virtual bool
  current_max_100_serial_present ();

  virtual short
  current_max_100_serial ();

  virtual bool
  current_min_100_parallel_present ();

  virtual short
  current_min_100_parallel ();

  virtual bool
  current_max_100_parallel_present ();

  virtual short
  current_max_100_parallel ();

  virtual bool
  pressure_tolerance_present ();

  virtual signed char
  pressure_tolerance ();

  virtual bool
  fan_current_gain_present ();

  virtual signed char
  fan_current_gain ();

  virtual bool
  fan_current_present ();

  virtual short
  fan_current ();

  virtual bool
  pump_current_present ();

  virtual short
  pump_current ();

  virtual bool
  pump_threshold_present ();

  virtual short
  pump_threshold ();

  virtual bool
  enabled_present ();

  virtual signed char
  enabled ();

  virtual bool
  inactiv_shutdown_present ();

  virtual signed char
  inactiv_shutdown ();

  virtual bool
  inactiv_emcy_present ();

  virtual signed char
  inactiv_emcy ();

  virtual bool
  outval_inactiv_present ();

  virtual signed char
  outval_inactiv ();

  virtual bool
  livetime_limit_present ();

  virtual signed char
  livetime_limit ();

  virtual bool
  timestamp_present ();

  virtual signed char
  timestamp ();

  virtual bool
  threshold_present ();

  virtual signed char
  threshold ();

  virtual bool
  interval_present ();

  virtual signed char
  interval ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct configurationType_simpl_state
  {
    const ::configurationType* configurationType_;
  };

  configurationType_simpl_state configurationType_simpl_state_;
};

class limitswitchType_simpl: public limitswitchType_sskel
{
  public:
  virtual void
  pre (const ::limitswitchType&);

  // Attributes.
  //
  virtual bool
  index_present ();

  virtual signed char
  index ();

  // Elements.
  //
  virtual const ::configurationType&
  configuration ();

  public:
  struct limitswitchType_simpl_state
  {
    const ::limitswitchType* limitswitchType_;
  };

  limitswitchType_simpl_state limitswitchType_simpl_state_;
};

class position_codeType_simpl: public position_codeType_sskel
{
  public:
  position_codeType_simpl ();

  virtual void
  pre (const ::position_codeType&);

  // Attributes.
  //
  virtual bool
  value_present ();

  virtual signed char
  value ();

  virtual bool
  stop_present ();

  virtual signed char
  stop ();

  virtual bool
  stop_dir_present ();

  virtual ::std::string
  stop_dir ();

  virtual bool
  position_present ();

  virtual signed char
  position ();

  virtual bool
  width_present ();

  virtual signed char
  width ();

  virtual bool
  deviation_present ();

  virtual signed char
  deviation ();

  virtual bool
  dir_check_present ();

  virtual signed char
  dir_check ();

  virtual bool
  hit_skip_present ();

  virtual signed char
  hit_skip ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct position_codeType_simpl_state
  {
    const ::position_codeType* position_codeType_;
  };

  position_codeType_simpl_state position_codeType_simpl_state_;
};

class limitswitchesType_simpl: public limitswitchesType_sskel
{
  public:
  virtual void
  pre (const ::limitswitchesType&);

  // Elements.
  //
  virtual bool
  limitswitch_next ();

  virtual const ::limitswitchType&
  limitswitch ();

  virtual bool
  position_code_next ();

  virtual const ::position_codeType&
  position_code ();

  public:
  struct limitswitchesType_simpl_state
  {
    const ::limitswitchesType* limitswitchesType_;
    ::limitswitchesType::limitswitch_const_iterator limitswitch_;
    ::limitswitchesType::limitswitch_const_iterator limitswitch_end_;
    ::limitswitchesType::position_code_const_iterator position_code_;
    ::limitswitchesType::position_code_const_iterator position_code_end_;
  };

  limitswitchesType_simpl_state limitswitchesType_simpl_state_;
};

class position_coverageType_simpl: public position_coverageType_sskel
{
  public:
  virtual void
  pre (const ::position_coverageType&);

  // Elements.
  //
  virtual const ::encoderType&
  encoder ();

  virtual const ::limitswitchesType&
  limitswitches ();

  public:
  struct position_coverageType_simpl_state
  {
    const ::position_coverageType* position_coverageType_;
  };

  position_coverageType_simpl_state position_coverageType_simpl_state_;
};

class supervisionType_simpl: public supervisionType_sskel
{
  public:
  supervisionType_simpl ();

  virtual void
  pre (const ::supervisionType&);

  // Attributes.
  //
  virtual bool
  steploss_warn_limit_present ();

  virtual signed char
  steploss_warn_limit ();

  virtual bool
  steploss_error_limit_present ();

  virtual signed char
  steploss_error_limit ();

  virtual bool
  current_limit_present ();

  virtual short
  current_limit ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct supervisionType_simpl_state
  {
    const ::supervisionType* supervisionType_;
  };

  supervisionType_simpl_state supervisionType_simpl_state_;
};

class driverType_simpl: public driverType_sskel
{
  public:
  driverType_simpl ();

  virtual void
  pre (const ::driverType&);

  // Attributes.
  //
  virtual bool
  type_present ();

  virtual ::std::string
  type ();

  virtual bool
  reg_chopConf_present ();

  virtual ::std::string
  reg_chopConf ();

  virtual bool
  reg_smartEn_present ();

  virtual ::std::string
  reg_smartEn ();

  virtual bool
  reg_sgcsConf_present ();

  virtual ::std::string
  reg_sgcsConf ();

  virtual bool
  reg_drvConf_present ();

  virtual ::std::string
  reg_drvConf ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct driverType_simpl_state
  {
    const ::driverType* driverType_;
  };

  driverType_simpl_state driverType_simpl_state_;
};

class motion_profileType_simpl: public motion_profileType_sskel
{
  public:
  motion_profileType_simpl ();

  virtual void
  pre (const ::motion_profileType&);

  // Attributes.
  //
  virtual bool
  speed_min_present ();

  virtual signed char
  speed_min ();

  virtual bool
  speed_max_present ();

  virtual short
  speed_max ();

  virtual bool
  acc_present ();

  virtual short
  acc ();

  virtual bool
  dec_present ();

  virtual short
  dec ();

  virtual bool
  acc_time_present ();

  virtual signed char
  acc_time ();

  virtual bool
  dec_time_present ();

  virtual signed char
  dec_time ();

  virtual bool
  micro_steps_present ();

  virtual signed char
  micro_steps ();

  virtual bool
  ramp_type_present ();

  virtual signed char
  ramp_type ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct motion_profileType_simpl_state
  {
    const ::motion_profileType* motion_profileType_;
  };

  motion_profileType_simpl_state motion_profileType_simpl_state_;
};

class motion_profilesType_simpl: public motion_profilesType_sskel
{
  public:
  virtual void
  pre (const ::motion_profilesType&);

  // Elements.
  //
  virtual bool
  motion_profile_next ();

  virtual const ::motion_profileType&
  motion_profile ();

  public:
  struct motion_profilesType_simpl_state
  {
    const ::motion_profilesType* motion_profilesType_;
    ::motion_profilesType::motion_profile_const_iterator motion_profile_;
    ::motion_profilesType::motion_profile_const_iterator motion_profile_end_;
  };

  motion_profilesType_simpl_state motion_profilesType_simpl_state_;
};

class pid_controllerType_simpl: public pid_controllerType_sskel
{
  public:
  pid_controllerType_simpl ();

  virtual void
  pre (const ::pid_controllerType&);

  // Attributes.
  //
  virtual bool
  max_temperature_present ();

  virtual short
  max_temperature ();

  virtual bool
  controller_gain_present ();

  virtual short
  controller_gain ();

  virtual bool
  reset_time_present ();

  virtual short
  reset_time ();

  virtual bool
  derivative_time_present ();

  virtual signed char
  derivative_time ();

  virtual bool
  max_pressure_present ();

  virtual signed char
  max_pressure ();

  virtual bool
  min_pressure_present ();

  virtual signed char
  min_pressure ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct pid_controllerType_simpl_state
  {
    const ::pid_controllerType* pid_controllerType_;
  };

  pid_controllerType_simpl_state pid_controllerType_simpl_state_;
};

class pid_controllersType_simpl: public pid_controllersType_sskel
{
  public:
  virtual void
  pre (const ::pid_controllersType&);

  // Elements.
  //
  virtual const ::pid_controllerType&
  pid_controller ();

  public:
  struct pid_controllersType_simpl_state
  {
    const ::pid_controllersType* pid_controllersType_;
  };

  pid_controllersType_simpl_state pid_controllersType_simpl_state_;
};

class pwm_controllerType_simpl: public pwm_controllerType_sskel
{
  public:
  pwm_controllerType_simpl ();

  virtual void
  pre (const ::pwm_controllerType&);

  // Attributes.
  //
  virtual bool
  max_actuating_value_present ();

  virtual short
  max_actuating_value ();

  virtual bool
  min_actuating_value_present ();

  virtual short
  min_actuating_value ();

  virtual bool
  max_pwm_duty_present ();

  virtual signed char
  max_pwm_duty ();

  virtual bool
  min_pwm_duty_present ();

  virtual signed char
  min_pwm_duty ();

  public:
  ::xml_schema::string_simpl base_impl_;

  public:
  struct pwm_controllerType_simpl_state
  {
    const ::pwm_controllerType* pwm_controllerType_;
  };

  pwm_controllerType_simpl_state pwm_controllerType_simpl_state_;
};

class functionmoduleType_simpl: public functionmoduleType_sskel
{
  public:
  virtual void
  pre (const ::functionmoduleType&);

  // Attributes.
  //
  virtual bool
  type_present ();

  virtual ::std::string
  type ();

  virtual bool
  key_present ();

  virtual ::std::string
  key ();

  virtual bool
  name_present ();

  virtual ::std::string
  name ();

  virtual bool
  interface_present ();

  virtual signed char
  interface ();

  virtual bool
  fct_instanceID_present ();

  virtual ::std::string
  fct_instanceID ();

  // Elements.
  //
  virtual bool
  rotation_present ();

  virtual const ::rotationType&
  rotation ();

  virtual bool
  reference_run_present ();

  virtual const ::reference_runType&
  reference_run ();

  virtual bool
  position_coverage_present ();

  virtual const ::position_coverageType&
  position_coverage ();

  virtual bool
  supervision_present ();

  virtual const ::supervisionType&
  supervision ();

  virtual bool
  driver_present ();

  virtual const ::driverType&
  driver ();

  virtual bool
  motion_profiles_present ();

  virtual const ::motion_profilesType&
  motion_profiles ();

  virtual bool
  configuration_present ();

  virtual const ::configurationType&
  configuration ();

  virtual bool
  pid_controllers_present ();

  virtual const ::pid_controllersType&
  pid_controllers ();

  virtual bool
  pwm_controller_present ();

  virtual const ::pwm_controllerType&
  pwm_controller ();

  public:
  struct functionmoduleType_simpl_state
  {
    const ::functionmoduleType* functionmoduleType_;
  };

  functionmoduleType_simpl_state functionmoduleType_simpl_state_;
};

class functionmodulesType_simpl: public functionmodulesType_sskel
{
  public:
  virtual void
  pre (const ::functionmodulesType&);

  // Elements.
  //
  virtual bool
  functionmodule_next ();

  virtual const ::functionmoduleType&
  functionmodule ();

  public:
  struct functionmodulesType_simpl_state
  {
    const ::functionmodulesType* functionmodulesType_;
    ::functionmodulesType::functionmodule_const_iterator functionmodule_;
    ::functionmodulesType::functionmodule_const_iterator functionmodule_end_;
  };

  functionmodulesType_simpl_state functionmodulesType_simpl_state_;
};

class slaveType_simpl: public slaveType_sskel
{
  public:
  virtual void
  pre (const ::slaveType&);

  // Attributes.
  //
  virtual bool
  type_present ();

  virtual ::std::string
  type ();

  virtual bool
  key_present ();

  virtual ::std::string
  key ();

  virtual bool
  name_present ();

  virtual ::std::string
  name ();

  virtual bool
  virtual__present ();

  virtual signed char
  virtual_ ();

  // Elements.
  //
  virtual signed char
  nodetype ();

  virtual signed char
  nodeindex ();

  virtual const ::functionmodulesType&
  functionmodules ();

  public:
  struct slaveType_simpl_state
  {
    const ::slaveType* slaveType_;
  };

  slaveType_simpl_state slaveType_simpl_state_;
};

class parameter_slavesType_simpl: public parameter_slavesType_sskel
{
  public:
  virtual void
  pre (const ::parameter_slavesType&);

  // Elements.
  //
  virtual bool
  slave_next ();

  virtual const ::slaveType&
  slave ();

  public:
  struct parameter_slavesType_simpl_state
  {
    const ::parameter_slavesType* parameter_slavesType_;
    ::parameter_slavesType::slave_const_iterator slave_;
    ::parameter_slavesType::slave_const_iterator slave_end_;
  };

  parameter_slavesType_simpl_state parameter_slavesType_simpl_state_;
};

class hwconfigType_simpl: public hwconfigType_sskel
{
  public:
  virtual void
  pre (const ::hwconfigType&);

  // Attributes.
  //
  virtual bool
  version_present ();

  virtual float
  version ();

  // Elements.
  //
  virtual const ::parameter_masterType&
  parameter_master ();

  virtual const ::parameter_slavesType&
  parameter_slaves ();

  virtual const ::devicesType&
  devices ();

  public:
  struct hwconfigType_simpl_state
  {
    const ::hwconfigType* hwconfigType_;
  };

  hwconfigType_simpl_state hwconfigType_simpl_state_;
};

#ifdef HWCONFIG_SIMPL_HPP_CLEAR_OMIT_SAGGR
#  undef XSDE_OMIT_SAGGR
#endif

#ifndef XSDE_OMIT_SAGGR

// Serializer aggregate for the hwconfig element.
//
class hwconfig_saggr
{
  public:
  hwconfig_saggr ();

  void
  pre (const ::hwconfigType& x)
  {
    this->hwconfigType_s_.pre (x);
  }

  void
  post ()
  {
    this->hwconfigType_s_.post ();
  }

  ::hwconfigType_simpl&
  root_serializer ()
  {
    return this->hwconfigType_s_;
  }

  static const char*
  root_name ();

  static const char*
  root_namespace ();

  ::xml_schema::serializer_error
  _error ()
  {
    return this->hwconfigType_s_._error ();
  }

  void
  reset ()
  {
    this->hwconfigType_s_._reset ();
  }

  public:
  ::xml_schema::byte_simpl byte_s_;
  ::xml_schema::float_simpl float_s_;
  ::can_interfaceType_simpl can_interfaceType_s_;
  ::tcp_interfaceType_simpl tcp_interfaceType_s_;
  ::serial_numberType_simpl serial_numberType_s_;
  ::deviceType_simpl deviceType_s_;
  ::xml_schema::int_simpl int_s_;
  ::devicesType_simpl devicesType_s_;
  ::retortType_simpl retortType_s_;
  ::retortsType_simpl retortsType_s_;
  ::parameter_masterType_simpl parameter_masterType_s_;
  ::rotationType_simpl rotationType_s_;
  ::reference_runType_simpl reference_runType_s_;
  ::encoderType_simpl encoderType_s_;
  ::configurationType_simpl configurationType_s_;
  ::limitswitchType_simpl limitswitchType_s_;
  ::position_codeType_simpl position_codeType_s_;
  ::limitswitchesType_simpl limitswitchesType_s_;
  ::position_coverageType_simpl position_coverageType_s_;
  ::supervisionType_simpl supervisionType_s_;
  ::driverType_simpl driverType_s_;
  ::xml_schema::string_simpl string_s_;
  ::xml_schema::short_simpl short_s_;
  ::motion_profileType_simpl motion_profileType_s_;
  ::motion_profilesType_simpl motion_profilesType_s_;
  ::functionmoduleType_simpl functionmoduleType_s_;
  ::pid_controllerType_simpl pid_controllerType_s_;
  ::pid_controllersType_simpl pid_controllersType_s_;
  ::functionmodulesType_simpl functionmodulesType_s_;
  ::slaveType_simpl slaveType_s_;
  ::pwm_controllerType_simpl pwm_controllerType_s_;
  ::parameter_slavesType_simpl parameter_slavesType_s_;
  ::hwconfigType_simpl hwconfigType_s_;
};

#endif // XSDE_OMIT_SAGGR

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // HWCONFIG_SIMPL_HPP
