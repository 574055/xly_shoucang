#ifndef WEIGHT_TRACKER_H
#define WEIGHT_TRACKER_H

#include "queueset.h"
#include "rfid.h"
#include "weight.h"


enum ItemState
{
    ItemGone,     // Item is not detected by RFID.
    ItemDetected, // Item is detected by RFID, but it's off the tray.
    ItemOnTray,   // Item is detected by RFID and on the tray.
};


/**
 * Class responsible for tracking weights of each tag. Each tag is associated
 * with a weight, calculated when the tagged item is placed on the tray.
 */
class WeightTracker
{
public:
    rfidmap<float> rfidWeights;    // Stores tag-weight associations.
    rfidmap<ItemState> rfidStates; // Stores tag-state associations.

    rfidmap<int16_t> prevRSSI;                 // Tracks previous RSSI for comparison across iterations.
    queueset<RFIDIdentifier, 4> trayedQueue;   // Queues deposited items while waiting for weight increase.
    queueset<RFIDIdentifier, 4> untrayedQueue; // Queues deposited items while waiting for weight decrease.

    const RFIDAggregator& rfid;

    WeightTracker(const RFIDAggregator& rfid) : rfid{rfid} {}

    /**
     * Updates the weight and state associated with each tag.
     * This is the primary function for all the weight associating.
     */
    void update(float weight);

    /**
     * Return the total weight currently on the tray.
     */
    float total() const
    {
        float sum = 0;
        for (const auto& pr : rfidWeights)
            if (rfidStates.at(pr.first) == ItemOnTray)
                sum += pr.second;
        return sum;
    }

    /**
     * Get items matching a certain state.
     */
    rfidset getItems(ItemState state) const
    {
        rfidset set;
        for (const auto& pr : rfidStates) {
            if (pr.second == state)
                set.insert(pr.first);
        }
        return set;
    }

    /**
     * Count items matching a certain state.
     */
    size_t countItems(ItemState state) const
    {
        size_t count = 0;
        for (const auto& pr : rfidStates) {
            if (pr.second == state)
                count++;
        }
        return count;
    }

    /**
     * Get the RFID tags that are on the tray.
     */
    rfidset onTray() const
    {
        return getItems(ItemOnTray);
    }

    /**
     * Get the weight associated with an RFID. Returns 0 if not found.
     */
    float getWeight(const RFIDIdentifier& id) const
    {
        auto it = rfidWeights.find(id);
        return it == rfidWeights.end() ? 0.0f : it->second;
    }

    void log() const;

private:
    /**
     * Helper functions for updating the state of an ID.
     */
    // Mark an ID as gone.
    void markAsGone(const RFIDIdentifier& id);
    // Mark an ID as detected.
    void markAsDetected(const RFIDIdentifier& id);
    // Mark an ID as on the tray, and associate it with a weight.
    void markAsTrayed(const RFIDIdentifier& id, float weight);
    // Mark an ID as detected by RSSI, but waiting for a weight update.
    void queueForTraying(const RFIDIdentifier& id);
    void queueForUntraying(const RFIDIdentifier& id);

    /**
     * Logic helpers.
     */
    // Cross-check new IDs and existing IDs and mark new IDs appropriately.
    void detectNewIDs();
    // Update the prevRSSI of ids.
    void updateRSSI();
    // Cross-check existing IDs with new IDs and mark missing IDs appropriately.
    void handleMissingIDs();

    ItemState getState(const RFIDIdentifier& id) const
    {
        auto it = rfidStates.find(id);
        return it == rfidStates.end() ? ItemGone : it->second;
    }

    int16_t getPrevRSSI(const RFIDIdentifier& id) const
    {
        auto it = prevRSSI.find(id);
        return it == prevRSSI.end() ? 0 : it->second;
    }
};


#endif