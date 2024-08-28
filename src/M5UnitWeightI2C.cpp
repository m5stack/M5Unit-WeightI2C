#include "M5UnitWeightI2C.h"

bool M5UnitWeightI2C::begin(TwoWire *wire, uint8_t sda, uint8_t scl,
                            uint8_t addr, uint32_t speed) {
    _wire  = wire;
    _addr  = addr;
    _sda   = sda;
    _scl   = scl;
    _speed = speed;
    _wire->begin(_sda, _scl);
    _wire->setClock(_speed);
    delay(10);
    _wire->beginTransmission(_addr);
    uint8_t error = _wire->endTransmission();
    if (error == 0) {
        return true;
    } else {
        return false;
    }
}

bool M5UnitWeightI2C::writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                                 uint8_t length) {
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->write(buffer, length);
    if (_wire->endTransmission() == 0) return true;
    return false;
}

bool M5UnitWeightI2C::readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                                uint8_t length) {
    uint8_t index = 0;
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    if (_wire->requestFrom(addr, length)) {
        for (uint8_t i = 0; i < length; i++) {
            buffer[index++] = _wire->read();
        }
        return true;
    }
    return false;
}

bool M5UnitWeightI2C::setLPFilter(uint8_t en) {
    uint8_t reg = WEIGHT_I2C_FILTER_REG;

    return writeBytes(_addr, reg, (uint8_t *)&en, 1);
}

uint8_t M5UnitWeightI2C::getLPFilter(void) {
    uint8_t data;
    uint8_t reg = WEIGHT_I2C_FILTER_REG;

    readBytes(_addr, reg, (uint8_t *)&data, 1);

    return data;
}

bool M5UnitWeightI2C::setAvgFilter(uint8_t avg) {
    uint8_t reg = WEIGHT_I2C_FILTER_REG + 1;

    return writeBytes(_addr, reg, (uint8_t *)&avg, 1);
}

uint8_t M5UnitWeightI2C::getAvgFilter(void) {
    uint8_t data;
    uint8_t reg = WEIGHT_I2C_FILTER_REG + 1;

    readBytes(_addr, reg, (uint8_t *)&data, 1);

    return data;
}

bool M5UnitWeightI2C::setEmaFilter(uint8_t ema) {
    uint8_t reg = WEIGHT_I2C_FILTER_REG + 2;

    return writeBytes(_addr, reg, (uint8_t *)&ema, 1);
}

uint8_t M5UnitWeightI2C::getEmaFilter(void) {
    uint8_t data;
    uint8_t reg = WEIGHT_I2C_FILTER_REG + 2;

    readBytes(_addr, reg, (uint8_t *)&data, 1);

    return data;
}

float M5UnitWeightI2C::getWeight() {
    uint8_t data[4];
    float c;
    uint8_t *p;

    if (readBytes(_addr, WEIGHT_I2C_CAL_DATA_REG, data, 4)) {
        p = (uint8_t *)&c;
        memcpy(p, data, 4);
    };
    return c;
}

int32_t M5UnitWeightI2C::getWeightInt() {
    uint8_t data[4];

    readBytes(_addr, WEIGHT_I2C_CAL_DATA_INT_REG, data, 4);
    return (data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
}

String M5UnitWeightI2C::getWeightString() {
    char *p;
    uint8_t data[16];
    String res;

    readBytes(_addr, WEIGHT_I2C_CAL_DATA_STRING_REG, data, 16);
    p   = (char *)data;
    res = p;
    return res;
}

float M5UnitWeightI2C::getGapValue() {
    uint8_t data[4];
    float c;
    uint8_t *p;

    if (readBytes(_addr, WEIGHT_I2C_SET_GAP_REG, data, 4)) {
        p = (uint8_t *)&c;
        memcpy(p, data, 4);
    };
    return c;
}

void M5UnitWeightI2C::setGapValue(float offset) {
    uint8_t datatmp[4];
    uint8_t *p;
    p = (uint8_t *)&offset;

    memcpy(datatmp, p, 4);

    writeBytes(_addr, WEIGHT_I2C_SET_GAP_REG, datatmp, 4);
    delay(100);
}

void M5UnitWeightI2C::setOffset(void) {
    uint8_t datatmp[4];
    datatmp[0] = 1;

    writeBytes(_addr, WEIGHT_I2C_SET_OFFESET_REG, datatmp, 1);
}

int32_t M5UnitWeightI2C::getRawADC() {
    uint8_t data[4] = {0};
    int rawADC      = 0;
    if (readBytes(_addr, WEIGHT_I2C_RAW_ADC_REG, data, 4)) {
        rawADC = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    };

    return rawADC;
}

uint8_t M5UnitWeightI2C::setI2CAddress(uint8_t addr) {
    _wire->beginTransmission(_addr);
    _wire->write(I2C_ADDRESS_REG);
    _wire->write(addr);
    _wire->endTransmission();
    _addr = addr;
    return _addr;
}

uint8_t M5UnitWeightI2C::getI2CAddress(void) {
    _wire->beginTransmission(_addr);
    _wire->write(I2C_ADDRESS_REG);
    _wire->endTransmission();

    uint8_t RegValue;

    _wire->requestFrom(_addr, 1);
    RegValue = _wire->read();
    return RegValue;
}

uint8_t M5UnitWeightI2C::getFirmwareVersion(void) {
    _wire->beginTransmission(_addr);
    _wire->write(FIRMWARE_VERSION_REG);
    _wire->endTransmission();

    uint8_t RegValue;

    _wire->requestFrom(_addr, 1);
    RegValue = _wire->read();
    return RegValue;
}

void M5UnitWeightI2C::jumpBootloader(void) {
    uint8_t value = 1;

    writeBytes(_addr, JUMP_TO_BOOTLOADER_REG, (uint8_t *)&value, 1);
}
