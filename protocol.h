#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

/************************************** MultiWii Serial Protocol *******************************************************/
// Multiwii Serial Protocol 100
#define MSP_VERSION 100

//to multiwii developpers/committers : do not add new MSP messages without a proper argumentation/agreement on the forum
//range id [50-99] won't be assigned and can therefore be used for any custom multiwii fork without further MSP id conflict

#define MSP_PRIVATE 1 //in+out message      to be used for a generic framework : MSP + function code (LIST/GET/SET) + data. no code yet

#define MSP_TEST_ALTHOLD 10

#define MSP_IDENT 100      //out message         multitype + multiwii version + protocol version + capability variable
#define MSP_STATUS 101     //out message         cycletime & errors_count & sensor present & box activation & current setting number
#define MSP_RAW_IMU 102    //out message         9 DOF
#define MSP_SERVO 103      //out message         8 servos
#define MSP_MOTOR 104      //out message         8 motors
#define MSP_RC 105         //out message         8 rc chan and more
#define MSP_RAW_GPS 106    //out message         fix, numsat, lat, lon, alt, speed, ground course
#define MSP_COMP_GPS 107   //out message         distance home, direction home
#define MSP_ATTITUDE 108   //out message         2 angles 1 heading
#define MSP_ALTITUDE 109   //out message         altitude, variometer
#define MSP_ANALOG 110     //out message         vbat, powermetersum, rssi if available on RX
#define MSP_RC_TUNING 111  //out message         rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
#define MSP_PID 112        //out message         P I D coeff (9 are used currently)
#define MSP_BOX 113        //out message         BOX setup (number is dependant of your setup)
#define MSP_MISC 114       //out message         powermeter trig
#define MSP_MOTOR_PINS 115 //out message         which pins are in use for motors & servos, for GUI
#define MSP_BOXNAMES 116   //out message         the aux switch names
#define MSP_PIDNAMES 117   //out message         the PID names
#define MSP_WP 118         //out message         get a WP, WP# is in the payload, returns (WP#, lat, lon, alt, flags) WP#0-home, WP#16-poshold
#define MSP_BOXIDS 119     //out message         get the permanent IDs associated to BOXes
#define MSP_SERVO_CONF 120 //out message         Servo settings

#define MSP_NAV_STATUS 121 //out message         Returns navigation status
#define MSP_NAV_CONFIG 122 //out message         Returns navigation parameters

#define MSP_CELLS 130 //out message         FRSKY Battery Cell Voltages

#define MSP_SET_RAW_RC 200      //in message          8 rc chan
#define MSP_SET_RAW_GPS 201     //in message          fix, numsat, lat, lon, alt, speed
#define MSP_SET_PID 202         //in message          P I D coeff (9 are used currently)
#define MSP_SET_BOX 203         //in message          BOX setup (number is dependant of your setup)
#define MSP_SET_RC_TUNING 204   //in message          rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
#define MSP_ACC_CALIBRATION 205 //in message          no param
#define MSP_MAG_CALIBRATION 206 //in message          no param
#define MSP_SET_MISC 207        //in message          powermeter trig + 8 free for future use
#define MSP_RESET_CONF 208      //in message          no param
#define MSP_SET_WP 209          //in message          sets a given WP (WP#,lat, lon, alt, flags)
#define MSP_SELECT_SETTING 210  //in message          Select Setting Number (0-2)
#define MSP_SET_HEAD 211        //in message          define a new heading hold direction
#define MSP_SET_SERVO_CONF 212  //in message          Servo settings
#define MSP_SET_MOTOR 214       //in message          PropBalance function
#define MSP_SET_NAV_CONFIG 215  //in message          Sets nav config parameters - write to the eeprom

#define MSP_SET_ACC_TRIM 239 //in message          set acc angle trim values
#define MSP_ACC_TRIM 240     //out message         get acc angle trim values
#define MSP_BIND 241         //in message          no param
#define MSP_ARM 242          //in message
#define MSP_DIS_ARM 243      //in message
#define MSP_RAW_BARO 244     //out message
#define MSP_ALT_HOLD 245     // in message
#define MSP_ALT_UNLOCK 246   // in message

#define MSP_EEPROM_WRITE 250 //in message          no param

#define MSP_DEBUGMSG 253 //out message         debug string buffer
#define MSP_DEBUG 254    //out message         debug1,debug2,debug3,debug4


#define INPUT_BUFFER_SIZE 64
#define TX_BUFFER_SIZE 128

enum MSP_protocol_states
{
    IDLE,
    HEADER_START,
    HEADER_M,
    HEADER_ARROW,
    HEADER_SIZE,
    HEADER_CMD
};

struct StatusResponse
{
    uint16_t CycleTime, I2CErrorCount;
    bool SensorAcc, SensorGyro, SensorMag, SensorBaro, SensorSonar, SensorGPS;
    uint32_t Flag;
    uint8_t Set;
    void Unmarshel(const char *payload, uint8_t size);
};

struct RawIMUResponse
{
    int16_t Acc[3];
    int16_t Gyro[3];
    int16_t Mag[3];
    void Unmarshel(const char *payload, uint8_t size);
};

struct RawBaroResponse
{
    int16_t CT;
    int32_t CP;
    void Unmarshel(const char *payload, uint8_t size);
};

struct AttitudeResponse
{
    int16_t AngleX;
    int16_t AngleY;
    int16_t Heading;
    void Unmarshel(const char *payload, uint8_t size);

};

struct AltitudeResponse
{
    int32_t Alt;
    int16_t Vario;
    void Unmarshel(const char *payload, uint8_t size);
};

struct MotorResponse
{
    int16_t Motors[8];
    void Unmarshel(const char *payload, uint8_t size);
};

int32_t UnmarshelAltHold(const char *payload);

static uint8_t read8(const char *payload);
static uint16_t read16(const char *payload);
static uint32_t read32(const char *payload);

#endif // PROTOCOL_H
