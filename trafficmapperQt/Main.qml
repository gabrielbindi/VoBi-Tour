import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtLocation
import QtPositioning
import trafficmapperQt

ApplicationWindow{
    visible: true
    title: "Verkehrsmapper"
    Component.onCompleted: showFullScreen()

    Plugin {
        id: osmPlugin
        name: "osm"
    }

    TrafficFetcher {
        id: fetcher1
        Component.onCompleted: fetchSegment(47.0707, 15.4395)
    }

    TrafficFetcher {
        id: fetcher2
        Component.onCompleted: fetchSegment(47.0650, 15.4395)
    }

    TrafficFetcher {
        id: fetcher3
        Component.onCompleted: fetchSegment(47.0750, 15.4420)
    }

    MapView {
        id: view
        anchors.fill: parent
        map.plugin: osmPlugin
        map.center: QtPositioning.coordinate(47.07083, 15.43861) //Graz
        map.zoomLevel: 14

        MapPolyline { parent: view.map; line.width: 5; line.color: "red"; path: fetcher1.coordinates }
        MapPolyline { parent: view.map; line.width: 5; line.color: "blue"; path: fetcher2.coordinates }
        MapPolyline { parent: view.map; line.width: 5; line.color: "green"; path: fetcher3.coordinates }

        MapPolyline {
            parent: view.map
            line.width: 5
            line.color: "red"
            path: trafficFetcher.coordinates
        }
    }

    Text {
         anchors.top: parent.top
         anchors.left: parent.left
         color: "black"
         font.pixelSize: 20
         text: "Speed: " + trafficFetcher.currentSpeed + " | Coords: " + trafficFetcher.coordinates.length
     }
}
