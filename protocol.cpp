#include "protocol.h"

static uint8_t read8(const char *payload)
{
    return *payload;
}

static uint16_t read16(const char *payload)
{
    uint16_t c = read8(payload);
    payload++;
    c += (uint16_t)read8(payload) << 8;
    return c;
}

static uint32_t read32(const char *payload)
{
    uint32_t c = read16(payload);
    payload+=2;
    c += (uint32_t)read16(payload) << 16;
    return c;
}

void StatusResponse::Unmarshel(const char *payload, uint8_t size)
{
    if (size < 11)
    {
        return;
    }

    CycleTime = read16(payload);
    payload += 2;

    I2CErrorCount = read16(payload);
    payload += 2;

    uint16_t sensor = read16(payload);
    SensorAcc = sensor & 0x01;
    SensorBaro = sensor & (1 << 1);
    SensorMag = sensor & (1 << 2);
    SensorGPS = sensor & (1 << 3);
    SensorSonar = sensor & (1 << 4);
    SensorGyro = sensor & (1 << 5);
    payload += 2;

    Flag = read32(payload);
    payload += 4;

    Set = read8(payload);
}

void RawIMUResponse::Unmarshel(const char *payload, uint8_t size)
{
    if (size < 18)
    {
        return;
    }

    for (int i = 0; i < 6; i += 2)
    {
        Acc[i/2] = read16(payload);
        payload += 2;
    }
    for (int i = 0; i < 6; i += 2)
    {
        Gyro[i/2] = read16(payload);
        payload += 2;
    }
    for (int i = 0; i < 6; i += 2)
    {
        Mag[i/2] = read16(payload);
        payload += 2;
    }
}

void RawBaroResponse::Unmarshel(const char *payload, uint8_t size)
{
    if (size < 6)
    {
        return;
    }

    CT = read16(payload);
    payload += 2;
    CP = read32(payload);
}

void AttitudeResponse::Unmarshel(const char *payload, uint8_t size)
{
    if (size < 6)
    {
        return;
    }

    AngleX = read16(payload);
    payload += 2;

    AngleY = read16(payload);
    payload += 2;

    Heading = read16(payload);
}

void AltitudeResponse::Unmarshel(const char *payload, uint8_t size)
{
    if (size < 6)
    {
        return;
    }

    Alt = read32(payload);
    payload += 4;
    Vario = read16(payload);
}

void MotorResponse::Unmarshel(const char *payload, uint8_t size)
{
    if (size < 16)
    {
        return;
    }

    for (int i = 0; i < size; i += 2)
    {
        Motors[i/2] = read16(payload);
        payload += 2;
    }
}

int32_t UnmarshelAltHold(const char *payload)
{
    return read32(payload);
}
