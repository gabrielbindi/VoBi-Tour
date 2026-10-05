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
        id: trafficFetcher
        Component.onCompleted: fetchSegment(47.0707, 15.4395)
    }

    MapView {
        id: view
        anchors.fill: parent
        map.plugin: osmPlugin
        map.center: QtPositioning.coordinate(47.07083, 15.43861) //Graz
        map.zoomLevel: 15

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
