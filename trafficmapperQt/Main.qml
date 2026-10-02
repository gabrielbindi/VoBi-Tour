import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtLocation
import QtPositioning

ApplicationWindow{
    visible: true
    width: showFullScreen()
    height: showFullScreen()
    title: "map"

    Plugin {
        id: osmPlugin
        name: "osm"
    }

    MapView {
        anchors.fill: parent
        map.plugin: osmPlugin
        map.center: QtPositioning.coordinate(47.07083, 15.43861) //Graz
        map.zoomLevel: 12
    }
}
