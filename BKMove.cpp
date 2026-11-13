#include "BKMove.h"

void BKMove::addRoute(BusRoute* route) {
    if (route == nullptr) throw invalid_argument("route must not be null");
    routes.push_back(route);
}

int BKMove::getRouteCount() const {
    return static_cast<int>(routes.size());
}

BusRoute* BKMove::getRoute(int index) {
    if (index < 0 || index >= static_cast<int>(routes.size())) {
        throw out_of_range("BKMove route index is out of range");
    }
    return routes[index];
}
int compareDirectRoutes (RouteResult& lhs, RouteResult& rhs) {
    if (lhs.hopCount<rhs.hopCount) return -1;
    if (lhs.hopCount>rhs.hopCount) return +1;
    if (lhs.routeId<rhs.routeId) return -1;
    if (lhs.routeId>rhs.routeId) return +1;
    if (lhs.direction<rhs.direction) return -1;
    if (lhs.direction>rhs.direction) return +1;
    return 0;
}
vector<RouteResult> BKMove::findDirectRoutes(string fromStopId, string toStopId) {
    vector<RouteResult> RouteList;
    for (size_t i=0; i<routes.size(); i++) {
        BusRoute* route=routes[i];
        int outboundRoutes= route->getHopCount(fromStopId, toStopId, OUTBOUND);
        int inboundRoutes= route->getHopCount(fromStopId, toStopId, INBOUND);
        if (outboundRoutes>=0) RouteList.push_back(RouteResult(route->getId(), OUTBOUND, outboundRoutes));
        if (inboundRoutes>=0) RouteList.push_back(RouteResult(route->getId(), INBOUND, inboundRoutes));
    }
    if (!RouteList.empty()) {
        QuickSort<RouteResult> SortTool;
        SortTool.sort(RouteList.data(), RouteList.size(), &compareDirectRoutes);
    }
    return RouteList;
}
int JourneyComparator(JourneyResult& a, JourneyResult& b) {
    if (a.transfers<b.transfers) return -1;
    if (a.transfers>b.transfers) return +1;
    if (a.totalHops<b.totalHops) return -1;
    if (a.totalHops>b.totalHops) return +1;
    if (a.firstRouteId<b.firstRouteId) return -1;
    if (a.firstRouteId>b.firstRouteId) return +1;
    if (a.firstDirection<b.firstDirection) return -1;
    if (a.firstDirection>b.firstDirection) return +1;
    if (a.transferStopId<b.transferStopId) return -1;
    if (a.transferStopId>b.transferStopId) return +1;
    if (a.secondRouteId<b.secondRouteId) return -1;
    if (a.secondRouteId>b.secondRouteId) return +1;
    if (a.secondDirection<b.secondDirection) return -1;
    if (a.secondDirection>b.secondDirection) return +1;
    return 0;
}
vector<JourneyResult> BKMove::findJourneys(string fromStopId, string toStopId) {
    vector<RouteResult> List=findDirectRoutes(fromStopId, toStopId);
    vector<JourneyResult> result;
    for(size_t i=0; i< List.size(); i++) {
        result.push_back(JourneyResult(List[i].routeId, List[i].direction, List[i].hopCount));
    }
    for(size_t i=0; i<routes.size(); i++) {
        for (Direction bound1: {OUTBOUND, INBOUND}) {
            for(int j=0; j<routes[i]->getStopCount(bound1); j++) {
                string transferStop= routes[i]->getStop(j, bound1).getId();
                int hop1=routes[i]->getHopCount(fromStopId, transferStop, bound1);
                if (hop1<=0) continue;
                for (size_t k=0; k<routes.size(); k++) {
                    if (routes[k]->getId()==routes[i]->getId()) continue;
                    for (Direction bound2: {OUTBOUND, INBOUND}) {
                        int hop2=routes[k]->getHopCount(transferStop, toStopId, bound2);
                        if (hop2>0) {
                            result.push_back(JourneyResult(routes[i]->getId() ,bound1,transferStop, routes[k]->getId(), bound2, hop1+hop2));
                        }
                    }
                }
            }
        }
    }
    if (!result.empty()) {
        QuickSort<JourneyResult> SortTool;
        SortTool.sort(result.data(), result.size(), &JourneyComparator);
    }
    return result;
}
