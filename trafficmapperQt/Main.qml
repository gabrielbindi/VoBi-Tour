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

    TrafficModel {
            id: trafficModel
            Component.onCompleted: refresh()
        }

    Timer {
        interval: 60000   // 1 Minute
        running: true
        repeat: true
        onTriggered: trafficModel.refresh()
    }

        MapView {
            id: view
            anchors.fill: parent
            map.plugin: osmPlugin
            map.center: QtPositioning.coordinate(47.0707, 15.4395)
            map.zoomLevel: 14

            MapItemView {
                parent: view.map
                model: trafficModel
                delegate: MapPolyline {
                    line.width: 5
                    line.color: speedRatio > 0.8 ? "green" : (speedRatio > 0.5 ? "yellow" : "red")
                    path: model.path
                }
            }
        }

    Text {
         anchors.top: parent.top
         anchors.left: parent.left
         color: "black"
         font.pixelSize: 20
         text: "Segments: " + trafficModel.rowCount()
    }
}
