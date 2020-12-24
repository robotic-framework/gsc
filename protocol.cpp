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
