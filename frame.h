#define PREAMB 170
#define SENSOR_FAIL 254
#define SIZE_FAIL 0
#define CRC_FAIL 255

typedef struct
{
    uint8_t preamb;
    uint8_t sensor;
    uint8_t axis;
    uint8_t checksum;
}reqFrame;

typedef struct{
    uint8_t preamb;
    uint8_t sensor;
    uint8_t size;
    uint8_t data[9];
    uint8_t checksum;
}resFrame;
