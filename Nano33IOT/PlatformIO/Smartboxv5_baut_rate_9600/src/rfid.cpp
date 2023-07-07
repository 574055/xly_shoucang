#include "rfid.h"

#ifdef CONTROLLER_XTRAY
#include "main.h"
#endif

#include <algorithm>
#include <etl/map.h>
#include <etl/priority_queue.h>


#define DEBUG_OPTION DEBUG_RFID
#include "debug.h"


bool RFIDCache::add(const RFIDData& d)
{
    RFIDIdentifier rfid(d);
    bool isNew = (lastseen.find(rfid) == lastseen.end());
    lastseen[rfid] = millis();
    if (isNew) {
        // Insert the new ID into the set of tags.
        activeTags.insert(rfid);
        rssiqs[rfid] = rssiq{d.rssi};
    } else {
        // Identifier already exists. Update the signal strength.
        rssiqs[rfid].push(d.rssi);
    }
    return isNew;
}

bool RFIDCache::prune()
{
    bool changed = false;
    // Check if any tags have expired.
    for (auto it = lastseen.begin(); it != lastseen.end();) {
        auto last_seen = it->second;
        if (isExpired(last_seen)) {
            // Expired? Remove the tag.
            activeTags.erase(it->first);
            rssiqs[it->first].clear(); // Don't completely erase from map. If a tag is gone, simply say the signal is 0.
            it = lastseen.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }
    return changed;
}

void RFIDCache::print() const
{
    uint32_t now = millis();
    debugf("tags:\n");
    if (lastseen.empty()) {
        debugf(" > (empty)\n");
    }
    for (const auto& kv : lastseen) {
        debugf(" > %08x (%3d), expiring in %.2fs\n", kv.first.id, rssiqs.at(kv.first).max(),
               (RFID_TAG_TIMEOUT + kv.second - now) / 1000.0);
    }
}


RFIDReader::RFIDReader(rfid_uart_t& serial) : m_serial{serial}, m_commands{serial} {}

#ifdef CONTROLLER_ESP
void RFIDReader::init(int rxPin, int txPin, int enPin)
{
    this->enPin = enPin;
    m_serial.begin(115200, SERIAL_8N1, rxPin, txPin);

    pinMode(enPin, OUTPUT);
    enable();

    cmd().power(26);
}
#else
void RFIDReader::init()
{
    m_serial.begin(115200);
    cmd().power(26);
}
#endif

etl::unordered_set<RFIDData, RFID_MAX_TAGS> RFIDReader::scan(uint32_t timeout)
{
    etl::unordered_set<RFIDData, RFID_MAX_TAGS> tagset;

    uint8_t buffer[RFID_PACKET_BUFFER_SIZE];
    size_t size = 0;
    RFIDData tag;

    cmd().multiscan(RFID_MULTISCAN_AMOUNT);

    uint32_t last_millis = millis();

    while (true) {
        // Read
        while (m_serial.available()) {
            uint8_t data = m_serial.read();
            // Ensure that the first byte is the start byte.
            if (size > 0 || (size == 0 && data == RFID_START_BYTE)) {
                buffer[size++] = data;
            }
            if (data == RFID_END_BYTE) {
                // debugf("Parsing packet: ");
                // for (int i = 0; i < size; i++)
                //     debugf("%02x", buffer[i]);
                // debugln();

                // Might be a packet? Let's try parsing it.
                switch (tag.parseFrom(buffer, size)) {
                    case RFIDParseError::OK:
                        // Handle the tag.
                        tagset.insert(tag);

                        // Clear buffer.
                        size = 0;

                        // if (optionGet(DEBUG_RFID))
                        //     tag.print();
                        break;

                    case RFIDParseError::IncorrectChecksum: debugln("bad checksum"); break;

                    case RFIDParseError::NotAPacket:
                    case RFIDParseError::ErrorPacket:
                    case RFIDParseError::OtherPacket:
                        // Nope. It wasn't a packet. Let's start over.
                        size = 0;
                        break;

                    case RFIDParseError::InsufficientData:
                        // Do nothing and continue from the current buffer.
                        break;
                }
            }
            if (size == RFID_PACKET_BUFFER_SIZE) {
                size = 0; // Restart.
            }
        }

        if (millis() > last_millis + timeout) {
            break;
        }

        delay(1);
    }

    return tagset;
}


RFIDAggregator::RFIDAggregator() = default;

void RFIDAggregator::add(RFIDReader* reader)
{
    if (m_numReader < RFID_MAX_READERS)
        m_reader[m_numReader++] = reader;
}

void RFIDAggregator::scan(uint32_t timeout)
{
    etl::unordered_set<RFIDData, RFID_MAX_TAGS> allTags;
    for (size_t i = 0; i < m_numReader; i++) {
        // Serial.printf("Reader %d:", i + 1);
        auto tags = m_reader[i]->scan(timeout);
        for (const auto& t : tags) {
            // Serial.printf(" %08x (%3d)", RFIDIdentifier(t).id, t.rssi);
            auto it = allTags.find(t);
            if (it == allTags.end()) {
                // If the tag hasn't been scanned this round, add it!
                allTags.insert(t);
            } else {
                // Otherwise, update the RSSI.
                it->rssi = max(it->rssi, t.rssi);
            }
        }
        // Serial.println();
    }

    for (const auto& t : allTags) {
        // Add the tags to the cache.
        if (m_cache.add(t)) {
            debugf("new tag: %08x\n", RFIDIdentifier(t).id);
        }
    }
}

void RFIDAggregator::prune()
{
    bool changed = m_cache.prune();
    (void*)changed; // Suppress unused variable warning.
}

void RFIDAggregator::loop()
{
    scan();
    prune();

    static uint32_t lt = 0;
    if (millis() > lt + 500) {
        lt = millis();
        m_cache.print();
    }
}

size_t RFIDAggregator::count() const
{
    return m_cache.count();
}

const rfidset& RFIDAggregator::tags() const
{
    return m_cache.activeTags;
}

int16_t RFIDAggregator::rssi(const RFIDIdentifier& rfid) const
{
    return m_cache.rssi(rfid);
}
