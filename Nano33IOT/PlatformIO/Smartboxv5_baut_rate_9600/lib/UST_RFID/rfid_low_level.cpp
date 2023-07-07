#include "rfid_low_level.h"

#include <Arduino.h>


void RFIDCommands::txpacket(const uint8_t bytes[], size_t size)
{
    tx(RFID_START_BYTE);
    uint8_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        tx(bytes[i]);
        checksum += bytes[i];
    }
    tx(checksum);
    tx(RFID_END_BYTE);
}

void RFIDCommands::scan()
{
    const uint8_t bytes[] = {0x00, 0x22, 0x00, 0x00, 0x22};
    txpacket(bytes, sizeof(bytes));
}

void RFIDCommands::multiscan(uint16_t n)
{
    const uint8_t bytes[] = {0x00, 0x27, 0x00, 0x03, 0x22, uint8_t((n >> 8) & 0xFF), uint8_t(n & 0xFF)};
    txpacket(bytes, sizeof(bytes));
}

void RFIDCommands::power(uint16_t dBm)
{
    uint16_t mBm = dBm * 100;
    const uint8_t bytes[] = {0x00, 0xB6, 0x00, 0x02, uint8_t((mBm >> 8) & 0xFF), uint8_t(mBm & 0xFF)};
    txpacket(bytes, sizeof(bytes));
}


bool verifyChecksum(const uint8_t bytes[], size_t size, uint8_t checksum)
{
    for (size_t i = 0; i < size; i++)
        checksum -= bytes[i];
    return checksum == 0;
}


RFIDParseError RFIDData::parseFrom(const uint8_t buffer[], size_t size)
{
    // Packets are typically at least 8 bytes (7 + 1+ packet data).
    if (size < 8)
        return RFIDParseError::InsufficientData;

    if (buffer[0] != RFID_START_BYTE || buffer[size - 1] != RFID_END_BYTE)
        return RFIDParseError::NotAPacket;

    // Parse packet length and ensure packet length checks out. This is important in case the data contains the end
    // byte.
    size_t packet_len = (buffer[3] << 8) | buffer[4];
    if (size < packet_len + 7) // Start byte, type, command, packet len (2), checksum, end byte.
        return RFIDParseError::InsufficientData;

    // This is an error packet. See page 10/11 of the datasheet.
    if (packet_len == 1 && buffer[5] == 0x15)
        return RFIDParseError::ErrorPacket;

    // Assume RFID tag packets are looonnnnnnng.
    if (packet_len < 12)
        return RFIDParseError::OtherPacket;

    // Ensure data isn't faulty.
    if (!verifyChecksum(buffer + 1, size - 3, buffer[size - 2]))
        return RFIDParseError::IncorrectChecksum;

    // Finally! We parse!
    rssi = buffer[5];
    pc = (buffer[6] << 8) | buffer[7];
    len = packet_len - 5;
    for (int i = 0; i < len; i++)
        epc[i] = buffer[8 + i];

    return RFIDParseError::OK;
}

void RFIDData::print() const
{
#ifdef CONTROLLER_ESP
    Serial.printf("len:%d,rssi:%d,pc:%d,", len, rssi, pc);
    Serial.print("epc:");
    for (auto x : epc) {
        Serial.printf("%02x", x);
    }
    Serial.println();
#else
    Serial.print("len:");
    Serial.print(len);
    Serial.print(",rssi:");
    Serial.print(rssi);
    Serial.print(",pc:");
    Serial.print(pc);
    Serial.print(",epc:");
    for (auto x : epc) {
        Serial.print((x >> 4) & 0xF, HEX);
        Serial.print(x & 0xF, HEX);
    }
    Serial.println();
#endif
}


RFIDIdentifier::RFIDIdentifier(const RFIDData& src)
{
#define SHIFT(x, b) (uint32_t(src.x) << b)
    id = src.len != 12 ? 0 : (SHIFT(epc[8], 24UL) | SHIFT(epc[9], 16UL) | SHIFT(epc[10], 8UL) | SHIFT(epc[11], 0));
}

void RFIDIdentifier::print(bool newline) const
{
#ifdef CONTROLLER_ESP
    Serial.printf(newline ? "%08x\n" : "%08x", id);
#else
    for (size_t i = 0; i < 8; i++) {
        Serial.print((id >> ((7 - i) * 4)) & 0xF, HEX);
    }
    if (newline)
        Serial.println();
#endif
}
