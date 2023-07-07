#ifndef RFID_H
#define RFID_H

// Import defines.
#if __has_include("defines.h")
#include "defines.h"
#else
#warning "Could not find a defines.h... using header defaults."
#define RFID_MAX_TAGS           64   // Maximum number of tags we can track.
#define RFID_SCAN_TIMEOUT       10   // Duration to wait before we stop the current scan iteration (in ms).
#define RFID_TAG_TIMEOUT        3000 // Duration to wait before a tag is discarded from the tracked tags (in ms).
#define RFID_MULTISCAN_AMOUNT   20   // Number of scans to perform for multiscan.
#define RFID_MAX_READERS        2    // Maximum number of RFIDReaders.
#define RFID_PACKET_BUFFER_SIZE 64   // Number of bytes for buffering a packet before parsing.
#endif

#include "rfid_low_level.h"

#include <Arduino.h>
#include <etl/unordered_map.h>
#include <etl/unordered_set.h>


namespace etl
{
    template <>
    struct hash<RFIDIdentifier>
    {
        size_t operator()(const RFIDIdentifier& id) const
        {
            return id.id;
        }
    };

    template <>
    struct hash<RFIDData>
    {
        size_t operator()(const RFIDData& data) const
        {
            size_t hash = 5381;
            for (char b : data.epc)
                hash = ((hash << 5) + hash) + b;
            return hash;
        }
    };
} // namespace etl


// Handy little typedefs for convenience.
template <typename T>
using rfidmap = etl::unordered_map<RFIDIdentifier, T, RFID_MAX_TAGS>;
using rfidset = etl::unordered_set<RFIDIdentifier, RFID_MAX_TAGS>;
using rssiq = byte_queue<uint32_t>;

/**
 * Keeps track of tags, recording when they were last seen and
 * discarding when they've expired.
 */
struct RFIDCache
{
    rfidmap<uint32_t> lastseen;
    rfidmap<rssiq> rssiqs;
    rfidset activeTags;

    bool add(const RFIDData& data);
    bool prune();

    bool isEmpty() const
    {
        return lastseen.empty();
    }
    size_t count() const
    {
        return lastseen.size();
    }

    int16_t rssi(const RFIDIdentifier& id) const
    {
        auto it = rssiqs.find(id);
        return it == rssiqs.end() ? 0 : it->second.max();
    }

    rfidset tags() const
    {
        return activeTags;
    }

    void print() const;

    static bool isExpired(uint32_t last_seen)
    {
        return millis() > last_seen + RFID_TAG_TIMEOUT;
    }
};


class RFIDReader
{
#ifdef CONTROLLER_ESP
    using uart_t = HardwareSerial;
#else
    using uart_t = Uart;
#endif
public:
    RFIDReader(uart_t& serial);

#ifdef CONTROLLER_ESP
    void init(int rxPin, int txPin, int enPin);
#else
    void init();
#endif

    void enable() const
    {
        digitalWrite(enPin, HIGH);
    }
    void disable() const
    {
        digitalWrite(enPin, LOW);
    }

    // Perform an RFID scan. Returns a set of data received.
    etl::unordered_set<RFIDData, RFID_MAX_TAGS> scan(uint32_t timeout);

private:
    uart_t& m_serial;
    RFIDCommands m_commands;
    int enPin = 0;

    RFIDCommands& cmd()
    {
        return m_commands;
    }
};


/**
 * High-level interface for scanning, updating internal RFID state, and accessing RFID data.
 */
class RFIDAggregator
{
public:
    RFIDAggregator();

    void add(RFIDReader* reader);

    // Scan for new tags and update the internal state.
    void scan(uint32_t timeout = RFID_SCAN_TIMEOUT);

    // Check if any RFID tags have expired (i.e. not seen for RFID_TAG_TIMEOUT time).
    void prune();

    // Scan, prune, and other internal logic.
    void loop();

    // Return the number of tags detected.
    size_t count() const;

    // Get the set of tags detected.
    const rfidset& tags() const;

    // Get the signal strength (RSSI) associated with an identifier.
    int16_t rssi(const RFIDIdentifier& rfid) const;

private:
    RFIDReader* m_reader[RFID_MAX_READERS] = {0};
    size_t m_numReader = 0;
    RFIDCache m_cache; // Caching logic.
};


#endif