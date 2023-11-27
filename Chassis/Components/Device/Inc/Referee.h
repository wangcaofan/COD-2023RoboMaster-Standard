#ifndef REFEREE_H
#define REFEREE_H
#include "string.h"
#include "stdio.h"
#include "crc.h"
#include "fifo.h"


#define HEADER_SOF 0xA5
#define REF_PROTOCOL_FRAME_MAX_SIZE         128

#define REF_PROTOCOL_HEADER_SIZE            sizeof(frame_header_struct_t)
#define REF_PROTOCOL_CMD_SIZE               2
#define REF_PROTOCOL_CRC16_SIZE             2
#define REF_HEADER_CRC_LEN                  (REF_PROTOCOL_HEADER_SIZE + REF_PROTOCOL_CRC16_SIZE)
#define REF_HEADER_CRC_CMDID_LEN            (REF_PROTOCOL_HEADER_SIZE + REF_PROTOCOL_CRC16_SIZE + sizeof(uint16_t))
#define REF_HEADER_CMDID_LEN                (REF_PROTOCOL_HEADER_SIZE + sizeof(uint16_t))

#pragma pack(push, 1)
typedef enum
{
  GAME_STATE_CMD_ID                 = 0x0001,
  GAME_RESULT_CMD_ID                = 0x0002,
  GAME_ROBOT_HP_CMD_ID              = 0x0003,
  FIELD_EVENTS_CMD_ID               = 0x0101,
  SUPPLY_PROJECTILE_ACTION_CMD_ID   = 0x0102,
  SUPPLY_PROJECTILE_BOOKING_CMD_ID  = 0x0103,
  REFEREE_WARNING_CMD_ID            = 0x0104,
  ROBOT_STATE_CMD_ID                = 0x0201,
  POWER_HEAT_DATA_CMD_ID            = 0x0202,
  ROBOT_POS_CMD_ID                  = 0x0203,
  BUFF_MUSK_CMD_ID                  = 0x0204,
  AERIAL_ROBOT_ENERGY_CMD_ID        = 0x0205,
  ROBOT_HURT_CMD_ID                 = 0x0206,
  SHOOT_DATA_CMD_ID                 = 0x0207,
  BULLET_REMAINING_CMD_ID           = 0x0208,
  ROBOT_RFID_STATE                  = 0x0209,
  DARTS_CLIENT_CMD_ID               = 0x020A,
  STUDENT_INTERACTIVE_DATA_CMD_ID   = 0x0301,
  IDCustomData,
}referee_cmd_id_t;

typedef  struct
{
  uint8_t SOF;
  uint16_t data_length;
  uint8_t seq;
  uint8_t CRC8;
}frame_header_struct_t;

typedef enum
{
  STEP_HEADER_SOF  = 0,
  STEP_LENGTH_LOW  = 1,
  STEP_LENGTH_HIGH = 2,
  STEP_FRAME_SEQ   = 3,
  STEP_HEADER_CRC8 = 4,
  STEP_DATA_CRC16  = 5,
}unpack_step_e;

typedef struct
{
  frame_header_struct_t *p_header;
  uint16_t       data_len;
  uint8_t        protocol_packet[REF_PROTOCOL_FRAME_MAX_SIZE];
  unpack_step_e  unpack_step;
  uint16_t       index;
}unpack_data_t;
#pragma pack(pop)

typedef enum
{
  RED_HERO        = 1,
  RED_ENGINEER    = 2,
  RED_STANDARD_1  = 3,
  RED_STANDARD_2  = 4,
  RED_STANDARD_3  = 5,
  RED_AERIAL      = 6,
  RED_SENTRY      = 7,
  BLUE_HERO       = 11,
  BLUE_ENGINEER   = 12,
  BLUE_STANDARD_1 = 13,
  BLUE_STANDARD_2 = 14,
  BLUE_STANDARD_3 = 15,
  BLUE_AERIAL     = 16,
  BLUE_SENTRY     = 17,
}robot_id_t;

typedef enum
{
  PROGRESS_UNSTART        = 0,
  PROGRESS_PREPARE        = 1,
  PROGRESS_SELFCHECK      = 2,
  PROGRESS_5sCOUNTDOWN    = 3,
  PROGRESS_BATTLE         = 4,
  PROGRESS_CALCULATING    = 5,
}game_progress_t;

typedef __packed struct //0001
{
  uint8_t game_type : 4;
  uint8_t game_progress : 4;
  uint16_t stage_remain_time;
}ext_game_state_t;

typedef __packed struct //0002
{
  uint8_t winner;
}ext_game_result_t;

typedef __packed struct
{
  uint16_t red_1_robot_HP;
  uint16_t red_2_robot_HP;
  uint16_t red_3_robot_HP;
  uint16_t red_4_robot_HP;
  uint16_t red_5_robot_HP;
  uint16_t red_7_robot_HP;
  uint16_t red_base_HP;
  uint16_t blue_1_robot_HP;
  uint16_t blue_2_robot_HP;
  uint16_t blue_3_robot_HP;
  uint16_t blue_4_robot_HP;
  uint16_t blue_5_robot_HP;
  uint16_t blue_7_robot_HP;
  uint16_t blue_base_HP;
}ext_game_robot_HP_t;

typedef __packed struct //0101
{
  uint32_t event_type;
}ext_event_data_t;

typedef __packed struct //0x0102
{
  uint8_t supply_projectile_id;
  uint8_t supply_robot_id;
  uint8_t supply_projectile_step;
  uint8_t supply_projectile_num;
}ext_supply_projectile_action_t;

typedef __packed struct //0x0103
{
  uint8_t supply_projectile_id;
  uint8_t supply_robot_id;
  uint8_t supply_num;
}ext_supply_projectile_booking_t;

typedef __packed struct
{
  uint8_t level;
  uint8_t foul_robot_id;
}ext_referee_warning_t;

typedef __packed struct //0x0201
{
  uint8_t robot_id;
  uint8_t robot_level;
  uint16_t remain_HP;
  uint16_t max_HP;
  uint16_t shooter_heat0_cooling_rate;
  uint16_t shooter_heat0_cooling_limit;
  uint16_t shooter_heat0_speed_limit;
  uint16_t shooter_heat1_cooling_rate;
  uint16_t shooter_heat1_cooling_limit;
  uint16_t shooter_heat1_speed_limit;
  uint16_t shooter_heat2_cooling_rate;
  uint16_t shooter_heat2_cooling_limit;
  uint16_t shooter_heat2_speed_limit;
  uint16_t chassis_power_limit;
  uint8_t mains_power_gimbal_output : 1;
  uint8_t mains_power_chassis_output : 1;
  uint8_t mains_power_shooter_output : 1;
}ext_game_robot_state_t;

typedef __packed struct //0x0202
{
  uint16_t chassis_volt;
  uint16_t chassis_current;
  float chassis_power;
  uint16_t chassis_power_buffer;
  uint16_t shooter_heat0;
  uint16_t shooter_heat1;
  uint16_t shooter_heat2;
}ext_power_heat_data_t;

typedef __packed struct //0x0203
{
  float x;
  float y;
  float z;
  float yaw;
}ext_game_robot_pos_t;

typedef __packed struct //0x0204
{
  uint8_t power_rune_buff;
}ext_buff_musk_t;

typedef __packed struct
{
 uint8_t attack_time;
}aerial_robot_energy_t;

typedef __packed struct //0x0206
{
  uint8_t armor_type : 4;
  uint8_t hurt_type : 4;
}ext_robot_hurt_t;

typedef __packed struct //0x0207
{
  uint8_t bullet_type;
  uint8_t shooter_id;
  uint8_t bullet_freq;
  float bullet_speed;
}ext_shoot_data_t;

typedef __packed struct //0x0208
{
  uint16_t bullet_remaining_num_17mm;
  uint16_t bullet_remaining_num_42mm;
  uint16_t coin_remaining_num;
}ext_bullet_remaining_t;

typedef __packed struct //0x0209
{
  uint32_t rfid_status;
}ext_rfid_status_t;

typedef __packed struct //0x020A
{
  uint8_t dart_launch_opening_status;
  uint8_t dart_attack_target;
  uint16_t target_change_time;
  uint16_t operate_launch_cmd_time;
}ext_dart_client_cmd_t;

typedef __packed struct //0x0301
{
  uint16_t send_ID;
  uint16_t receiver_ID;
  uint16_t data_cmd_id;
  uint16_t data_len;
  uint8_t *data;
}ext_student_interactive_data_t;

typedef __packed struct
{
  float data1;
  float data2;
  float data3;
  uint8_t data4;
}custom_data_t;

typedef __packed struct
{
  uint8_t data[64];
}ext_up_stream_data_t;

typedef __packed struct
{
  uint8_t data[32];
}ext_download_stream_data_t;

extern frame_header_struct_t referee_receive_header;
extern frame_header_struct_t referee_send_header;
extern ext_game_state_t game_state;
extern ext_game_result_t game_result;
extern ext_game_robot_HP_t game_robot_HP_t;
extern ext_event_data_t field_event;
extern ext_supply_projectile_action_t supply_projectile_action_t;
extern ext_supply_projectile_booking_t supply_projectile_booking_t;
extern ext_referee_warning_t referee_warning_t;
extern ext_game_robot_state_t robot_state;
extern ext_power_heat_data_t power_heat_data_t;
extern ext_game_robot_pos_t game_robot_pos_t;
extern ext_buff_musk_t buff_musk_t;
extern aerial_robot_energy_t robot_energy_t;
extern ext_robot_hurt_t robot_hurt_t;
extern ext_shoot_data_t shoot_data_t;
extern ext_bullet_remaining_t bullet_remaining_t;
extern ext_student_interactive_data_t student_interactive_data_t;


extern void init_referee_struct_data(void);
extern void referee_data_solve(uint8_t *frame);

extern void get_chassis_power_and_buffer(float *power, float *buffer);

extern uint8_t get_robot_id(void);

extern void get_shoot_heat0_limit_and_heat0(uint16_t *heat0_limit, uint16_t *heat0);
extern void get_shoot_heat1_limit_and_heat1(uint16_t *heat1_limit, uint16_t *heat1);

extern void REFEREE_IRQHandler_Called(void);
#endif
