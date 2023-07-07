#include "weight_tracking.h"

#include "queueset.h"
#include "rfid.h"


#define DEBUG_OPTION DEBUG_WEIGHT_TRACKER
#include "debug.h"

// TODO: perform stress test on weight tracking, by programmatically adding tons
// (on the order of 1000s) of labels, and ensuring the program doesn't break,
// and memory is sufficient.


static const char* STATE_STR[] = {"GONE", "DETECTED", "ON TRAY"};


#define INCREASE '+'
#define STABLE   '/'
#define DECREASE '-'

/**
 * Helper function which returns whether a value is increasing, stable, or decreasing.
 * A value is stable if it hasn't changed within a certain range.
 *
 * Formally, x is stable if |x - prev| < epsilon.
 */
template <typename T, typename U>
inline uint8_t diff(T x, T prev, U epsilon)
{
    return x > prev + epsilon ? INCREASE : x + epsilon < prev ? DECREASE : STABLE;
}


// Track the weights of each tag.
void WeightTracker::update(float weight)
{
    // 1. Detect new tags.
    // 2. Find any tags with a noticable jump in RSSI and queue them for traying.
    // 3. Update tag states.
    //  3.1. Weight change is stable, or
    //  3.2. Weight is increasing --> find which tag was added, or
    //  3.3. Weight is decreasing --> find which tag was taken.
    // 3. Update signal strength.
    // 4. Prune any missing RFIDs.

    const auto& newRFIDs = rfid.tags();

    // 0. Logging.
    log();

    // 1.
    detectNewIDs();

    // 2.
    for (const auto& pr : rfidStates) {
        const auto& id = pr.first;
        auto state = pr.second;
        auto prev = getPrevRSSI(id);
        auto curr = rfid.rssi(id);
        if (state == ItemGone || state == ItemDetected) {
            if (prev != 0 && curr > WT_ONTRAY_RSSI_THRESHOLD && curr - prev >= WT_ONTRAY_EPSILON) {
                queueForTraying(id);
            }
        } else if (state == ItemOnTray) {
            if (curr - prev < -WT_ONTRAY_EPSILON) {
                queueForUntraying(id);
            }
        }
    }

    // 3.
    float prevWeight = total();
    int wchange = diff(weight, prevWeight, WEIGHT_EPSILON);
    debugf("weight change: %.6f -> %.6fkg (%c)\n", prevWeight, weight, wchange);

    switch (wchange) {
        // 3.1.
        case STABLE:
            // Weight change is stable.
            break;

        // 3.2.
        case INCREASE:
            // Find items to put on the tray.
            if (!trayedQueue.empty()) {
                // Get the first element, and mark it as trayed.
                auto id = trayedQueue.front();
                debugf("popping from trayed %08x\n", id.id);
                markAsTrayed(id, weight - prevWeight);
                trayedQueue.pop();
            } else {
                // Sad. :(
                debugln("error: could not find a tag to associate with the weight increase");
            }
            break;

        // 3.3.
        case DECREASE:
            // Find items to remove from the tray.
            if (!untrayedQueue.empty()) {
                // Get the first element, and mark it as trayed.
                auto id = untrayedQueue.front();
                debugf("popping from untrayed %08x\n", id.id);
                bool isStillDetected = (newRFIDs.find(id) != newRFIDs.end());
                if (isStillDetected)
                    markAsDetected(id);
                else
                    markAsGone(id);
                untrayedQueue.pop();
            } else {
                // Sad. :(
                debugln("error: could not find a tag to associate with the weight decrease");
            }
            break;
    }

    // 3.
    updateRSSI();

    // 4.
    handleMissingIDs();
}

void WeightTracker::log() const
{
    debugln();
    debugln("weight associations:");
    for (const auto& pr : rfidStates) {
        const auto& id = pr.first;
        int16_t currRSSI = rfid.rssi(id);
        int16_t prevRSSI = getPrevRSSI(id);
        int change = diff(currRSSI, prevRSSI, 3);
        auto state = pr.second;
        debugf("%08x | %10s | %10.6fkg | rssi: %d -> %d (%c)\n", id, STATE_STR[state], getWeight(id), prevRSSI,
               currRSSI, change);
    }
}

void WeightTracker::markAsGone(const RFIDIdentifier& id)
{
    // rfidStates[id] = ItemGone;
    // prevRSSI[id] = 0;

    // See Note [Memory Saving]. Erase keys instead of saving keys and setting defaults.
    rfidStates.erase(id);
    prevRSSI.erase(id);
    rfidWeights.erase(id);
}

void WeightTracker::markAsDetected(const RFIDIdentifier& id)
{
    rfidStates[id] = ItemDetected;
}

void WeightTracker::markAsTrayed(const RFIDIdentifier& id, float weight)
{
    if (weight < 0) {
        // Bad bad.
        return;
    }
    rfidStates[id] = ItemOnTray;
    rfidWeights[id] = weight;
}

void WeightTracker::queueForTraying(const RFIDIdentifier& id)
{
    trayedQueue.push(id);
    debugf("queued for traying: %08x\n", id.id);
}

void WeightTracker::queueForUntraying(const RFIDIdentifier& id)
{
    untrayedQueue.push(id);
    debugf("queued for untraying: %08x\n", id.id);
}

void WeightTracker::detectNewIDs()
{
    for (const auto& id : rfid.tags()) {
        auto state = getState(id);
        if (state == ItemGone) {
            // It was lost (or non-existent), but now it's found!
            markAsDetected(id);
            prevRSSI[id] = rfid.rssi(id);
        }
    }
}

void WeightTracker::updateRSSI()
{
    for (const auto& id : rfid.tags()) {
        prevRSSI[id] = rfid.rssi(id);
    }
}

void WeightTracker::handleMissingIDs()
{
    for (auto& pr : rfidStates) {
        const auto& id = pr.first;
        auto& state = pr.second;
        if (state != ItemGone && rfid.tags().find(id) == rfid.tags().end()) {
            // Not found in the new ids? Sayonara!
            markAsGone(id);
        }
    }
}