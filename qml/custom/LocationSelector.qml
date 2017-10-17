/*
 OSMScout - a Qt backend for libosmscout and libosmscout-map
 Copyright (C) 2016  Lukas Karas

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

import QtQuick 2.0
import Sailfish.Silica 1.0

import QtPositioning 5.2

import harbour.osmscout.map 1.0

ComboBox {
    PositionSource {
        id: positionSource

        active: true

        property bool valid: false;
        property double lat: 0.0;
        property double lon: 0.0;

        onPositionChanged: {
            positionSource.valid = position.latitudeValid && position.longitudeValid;
            positionSource.lat = position.coordinate.latitude;
            positionSource.lon = position.coordinate.longitude;

            if (useCurrentLocation){
                location=routingModel.locationEntryFromPosition(positionSource.lat, positionSource.lon);
                console.log("Update and use current position: "+positionSource.lat + " " + positionSource.lon);
            }
        }
    }
    RoutingListModel{
        id: routingModel
    }

    id: selector

    property LocationEntry location: null
    property bool initialized: false
    property bool initWithCurrentLocation: false
    property bool useCurrentLocation: false

    property string selectLocationStr: qsTr("Select location...")
    property string currentLocationStr: qsTr("Current location")
    property string searchStr: qsTr("Search")

    signal selectLocation(LocationEntry loc)

    onSelectLocation: {
        console.log("selectLocation: " + loc);
        location=loc;
        value=location.label;
    }

    function activated(activeIndex){
        console.log("Activated, index: "+activeIndex);
        if (activeIndex==0){
            value=currentLocationStr;
            location=routingModel.locationEntryFromPosition(positionSource.lat, positionSource.lon);
            useCurrentLocation=true;
            console.log("Use current position: "+positionSource.lat + " " + positionSource.lon);
        }
        if (activeIndex==1){
            location=null; // in case of search cancel
            var searchPage=pageStack.push(Qt.resolvedUrl("../pages/Search.qml"),
                                          {
                                              searchCenterLat: positionSource.lat,
                                              searchCenterLon: positionSource.lon,
                                              acceptDestination: pageStack.currentPage
                                          });
            searchPage.selectLocation.connect(selectLocation);
            value=selectLocationStr;
            useCurrentLocation=false;
        }
    }

    value: selectLocationStr
    menu: ContextMenu {
        MenuItem { text: currentLocationStr }
        MenuItem { text: searchStr }
    }

    Connections {
        target: selector.menu
        onActivated: {
            activated(index);
        }
    }

    onCurrentItemChanged: {
        console.log("CurrentItemChanged, initialised: "+initialized+", index: "+currentIndex);
    }
    Component.onCompleted: {
        initialized = true;
        currentIndex = -1;
        console.log("onCompleted, initialised: "+initialized+", index: "+currentIndex);
        if (initWithCurrentLocation){
            if (positionSource.valid){
                currentIndex=0;
                activated(0);
            }else{
                console.log("Position is not valid yet")
            }
        }
    }
}
