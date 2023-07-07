#ifndef RFID_LOW_LEVEL_H
#define RFID_LOW_LEVEL_H

#ifdef CONTROLLER_ESP
#include <HardwareSerial.h>
#else
#include <Uart.h>
#endif
#include <cstdint>
#include <cstdlib>


#define RFID_START_BYTE 0xBB
#define RFID_END_BYTE   0x7E


#ifdef CONTROLLER_ESP
using rfid_uart_t = HardwareSerial;
#else
using rfid_uart_t = Uart;
#endif


/**
 * TX Commands to interface with the M100.
 */
class RFIDCommands
{
public:
    RFIDCommands(rfid_uart_t& serial) : m_serial{serial} {}

    void txpacket(const uint8_t bytes[], size_t size);
    void scan();
    void multiscan(uint16_t n);
    void power(uint16_t dBm);

private:
    rfid_uart_t& m_serial;

    void tx(uint8_t byte)
    {
        m_serial.write(byte);
    }

    uint8_t rx()
    {
        return m_serial.read();
    }
};

/**
 * Errors for parsing RFID.
 */
enum class RFIDParseError
{
    OK,                // Yep, it's an RFID packet.
    InsufficientData,  // More bytes are needed to determine if this is a packet or not.
    NotAPacket,        // This is definitely NOT a packet.
    ErrorPacket,       // This is a packet, but it indicates an error / nothing / null from the sensor.
    OtherPacket,       // This is a packet, but not an RFID tag packet.
    IncorrectChecksum, // This looks like a packet, but the checksum is wrong.
    // IncorrectCRC,
};

/**
 * A struct containing all the fields of an RFID packet
 */
struct RFIDData
{
    uint8_t rssi;    // Received Signal Strength Indicator.
    uint16_t pc;     // Product Code?
    uint8_t epc[12]; // Electronic Product Code.
    uint8_t len;     // Length of epc.

    RFIDParseError parseFrom(const uint8_t buffer[], size_t size);
    void print() const;

    friend bool operator==(const RFIDData& lhs, const RFIDData& rhs)
    {
        if (!(/* lhs.rssi == rhs.rssi &&  */ lhs.pc == rhs.pc && lhs.len == rhs.len))
            return false;

        for (uint8_t i = 0; i < lhs.len; i++)
            if (lhs.epc[i] != rhs.epc[i])
                return false;

        return true;
    }
};


#define DECL_BINARY_OP(op)                                                        \
    friend bool operator op(const RFIDIdentifier& lhs, const RFIDIdentifier& rhs) \
    {                                                                             \
        return lhs.id op rhs.id;                                                  \
    }

/**
 * A struct to store a unique 4-byte RFID value.
 */
// #define RFIDIdentiferNoValue 0xFFFFFFFF
struct RFIDIdentifier
{
    static constexpr uint32_t null = 0xFFFFFFFF;

    uint32_t id;

    RFIDIdentifier() : id{null} {}
    RFIDIdentifier(const RFIDData&);

    void print(bool newline = true) const;

    void clear()
    {
        id = null;
    }

    bool has_value() const
    {
        return id != null;
    }

    DECL_BINARY_OP(==)
    DECL_BINARY_OP(<)
};

#undef DECL_BINARY_OP


// Fast structure for queueing bytes using an int as the underlying container (e.g. uint32_t).
template <typename T = uint32_t>
struct byte_queue
{
    // Most recent byte is stored in LSB.
    T queue = 0;
    size_t len = 0;

    byte_queue(uint8_t x = 0) : queue{x} {}

    void push(uint8_t byte)
    {
        queue = (queue << 8) | byte;
        if (len < sizeof(T))
            len++;
    }

    void clear()
    {
        queue = 0;
        len = 0;
    }

    uint8_t max() const
    {
        uint8_t res = queue & 0xFF;
        T x = queue;
        for (size_t i = 1; i < len; i++) {
            x >>= 8;
            uint8_t cur = x & 0xFF;
            if (cur > res)
                res = cur;
        }
        return res;
    } 
};

#endif