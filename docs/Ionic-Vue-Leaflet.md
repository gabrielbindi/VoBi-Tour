= Open Street Map

<div id="map"> ist der leere Platz, in den Leaflet die Karte zeichnet. Leaflet ist kein Ionic-Baustein, sondern eine normale JavaScript-Bibliothek. Sie braucht ein normales HTML-Element und sucht es über die id.

import L from 'leaflet' holt die Bibliothek. L ist der Standardname, unter dem man Leaflet benutzt. Der Import von leaflet.css ist wichtig, sonst sieht die Karte kaputt aus.

L.map('map') erstellt eine leere Karte im <div> mit der id map. Diese Karte hat noch keinen Inhalt.

.setView([47.0707, 15.4395], 13) legt fest, wohin die Karte schaut: Koordinaten von Graz (Breite, Länge) und Zoomstufe 13. Kleinere Zahl heißt weiter weg, größere heißt näher dran.

L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png') sind die eigentlichen Kartenbilder. OpenStreetMap liefert die Karte in vielen kleinen Quadraten (Tiles, Kacheln). {z}/{x}/{y} sind Platzhalter, die Leaflet selbst ausfüllt, je nachdem, welcher Ausschnitt gerade sichtbar ist.

.addTo(map) legt die Kacheln auf die Karte. Ohne diese Zeile bleibt die Karte grau.

#map { height: 100% } gibt dem <div> eine Höhe. Ohne Höhe hat das <div> keine Größe, und man sieht nichts.

## Problem: Karte zeigt nur eine Kachel / ist grau

Das hatte bei mir zwei komplett getrennte Ursachen, die sich überlagert haben. Beide mussten gelöst werden, nicht nur eine.

### Ursache 1: Höhen-Kette fehlt

height: 100% funktioniert nur, wenn WIRKLICH JEDES Element von #map bis ganz nach oben (html) eine Höhe hat. Wenn irgendwo in der Kette die Höhe fehlt, rechnet der Browser mit height: auto, und das Element ist nur so groß wie sein Inhalt (bei einer leeren Karte: sehr klein, quasi 0).

Die Kette bei Ionic: html → body → #app → ion-app → ion-router-outlet → .ion-page → ion-content → #map

Karten-Seite:

```css
#map {
  height: 100%;
  width: 100%;
}
```

Wichtig: position: absolute mit top/left/right/bottom: 0 ist KEIN guter Ersatz dafür, wenn ion-content selbst kein position: relative hat. Dann rutscht die Karte aus dem sichtbaren Bereich raus und wird noch kleiner/unsichtbarer als vorher. Lieber die Höhen-Kette sauber machen.

### Ursache 2: Timing – Leaflet misst seine Größe nur EINMAL

Auch wenn das CSS komplett korrekt ist: Wenn L.map('map') aufgerufen wird, BEVOR der Browser das Layout fertig berechnet hat, denkt Leaflet in diesem Moment "ich bin winzig" und merkt sich das. Wird der Container danach größer, merkt Leaflet das nicht von alleine. Deshalb reicht korrektes CSS allein manchmal nicht.

Woran man das erkennt: Im Browser F12 → Netzwerk-Tab → nach tile.openstreetmap.org filtern. Wenn nur EINE einzige Kachel-Anfrage kommt, obwohl der Bildschirm groß ist, ist genau das der Grund.

Fix: Leaflet nach kurzer Zeit sagen "miss nochmal nach":

```ts
setTimeout(function () {
  if (map !== null) {
    map.invalidateSize();
  }
}, 300);
```

invalidateSize() zwingt Leaflet, die Containergröße neu abzufragen und ggf. mehr Kacheln nachzuladen.

Sauberer (aber komplizierter) wäre ein ResizeObserver statt setTimeout: Der beobachtet den Container dauerhaft und ruft invalidateSize() automatisch auf, sobald sich die Größe wirklich ändert — ohne eine Wartezeit raten zu müssen. Für den Anfang reicht setTimeout mit einem Wert wie 300 ms völlig aus. ResizeObserver ist ein Punkt für später, falls setTimeout auf einem langsamen Gerät mal zu kurz sein sollte.

### Merksatz

Zwei getrennte Fehlerquellen bei "Karte zu klein / grau / nur eine Kachel":
1. CSS-Höhen-Kette unvollständig → Karte hat 0 oder winzige Höhe
2. Leaflet wurde erstellt, bevor die Höhe feststand → Karte merkt sich die falsche (kleine) Größe dauerhaft

Man kann (2) nicht durch CSS allein lösen und (1) nicht durch invalidateSize() allein — beides muss stimmen.

### Wichtiger Debug-Schritt für nächstes Mal

Bei "ich sehe nichts" oder "es ist komisch": zuerst F12 → Konsole (rote Fehler?) und F12 → Netzwerk (werden die Kacheln überhaupt angefragt, und wie viele?). Das zeigt sofort, ob der Code überhaupt läuft (→ Timing-/Größenproblem) oder ob z. B. der Import fehlt oder eine Anfrage blockiert wird (→ anderer Fehler, z. B. Browser-Erweiterung).
