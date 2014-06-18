#ifndef MSG_H_
#define MSG_H_

#include "types.h"

enum ControlMsgType {
	//MSG_ARM,
	MSG_ACTION,
	MSG_CONTROL,
	MSG_DEBUG,
	MSG_PID
};

struct ControlMsg {
	ControlMsgType type;
};

/*struct MsgArm : ControlMsg {
	bool arm;
};*/

struct MsgControl : ControlMsg {
	uint16 throttle;
	int16 pitch, roll, yaw;
};

struct MsgDebug : ControlMsg {
	uint32 sendPeriod;
	uint8 logLevel;
};

enum PIDAxis{
	PID_PITCH,
	PID_ROLL,
	PID_YAW,
	PID_LEVEL,
	PID_MAG,
	PID_BARO,
	PID_AXIS_NUMBER
};

struct PID {
	float P, I, D;
};

struct MsgPID : ControlMsg {
	PID pid[PID_AXIS_NUMBER];
};

enum ControlAction{
	TERMINATE,
	ARM,
	DISARM,
	CALIBRATE_GYRO,
	CALIBRATE_ACCEL
};

struct MsgAction : ControlMsg {
	ControlAction action;
};

struct LogMsg {
	uint8 logLevel;
};

struct LogMsgText : LogMsg {
	char text;
};

struct LogMsg1 : LogMsg {
	// Level 1
	double rcThrottle, rcPitch, rcRoll, rcYaw;
	double pitch, roll, yaw;
	
};

struct LogMsg2 : LogMsg1 {
	// Level 2
	double interval;
	double motorFL, motorFR, motorBL, motorBR;
	double P[3], I[3], D[3];
	
};

struct LogMsg3 : LogMsg2 {
	// Level 3
	double fAccel[3], fGyro[3];
};

struct LogMsg4 : LogMsg3 {
	// Level 4
	double accel[3], gyro[3];
};

#endif