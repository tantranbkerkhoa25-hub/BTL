#include "BusRoute.h"

BusRoute::BusRoute(string routeId)
    : routeId(routeId), outboundCount(0), built(false) {}

string BusRoute::getId() const {
    return routeId;
}

bool BusRoute::isBuilt() const {
    return built;
}

int BusRoute::getStopCount(Direction direction) {
    if (!built) return 0;
    if (direction==OUTBOUND) {
        return this->outboundCount;
    }
    return stops.size()-outboundCount+2;
}

int BusRoute::physicalIndex(int index, Direction direction) {
    if (index<0 || index>=this->getStopCount(direction)) return -1;
    if (direction==OUTBOUND) return index;
    if (index==this->getStopCount(direction)-1) return 0;
    return index+outboundCount-1;
}

BusStop& BusRoute::getStop(int index, Direction direction) {
    if (index<0 || index>=getStopCount(direction)) throw std::out_of_range("Index out of bound");
    int realIdx=this->physicalIndex(index, direction);
    return stops.get(realIdx);
}

void BusRoute::build(SLinkedList<BusStop>& outbound, SLinkedList<BusStop>& inbound) {
    if (built) stops.clear();
    this->outboundCount=outbound.size();
    for (auto it=outbound.begin(); it!=outbound.end(); ++it) {
        BusStop station=*it;
        stops.add(station);
    }
    for (int i=1; i<inbound.size()-1; i++) {
        stops.add(inbound.get(i));
    }
    built=true;
}
int BusRoute::getHopCount(string fromStopId, string toStopId, Direction direction) {
    if (!built) return -1;
    int fromIdx=-1;
    int toIdx=-1;
    int stopCount = getStopCount(direction);
    for (int i=0; i<stopCount; i++) {
        string stopId = getStop(i, direction).getId();
        if (stopId == fromStopId) fromIdx = i;
        if (stopId == toStopId) toIdx = i;
    }
    if (fromIdx == -1 || toIdx == -1 || toIdx < fromIdx) {
        return -1;
    }
    return toIdx-fromIdx;
}
