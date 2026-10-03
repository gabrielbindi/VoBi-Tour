## Was ist eine Bounding Box?

Ein Rechteck, beschrieben durch zwei Koordinaten: die Südwest-Ecke und die Nordost-Ecke des sichtbaren Kartenbereichs. Damit kann man später einer API sagen: "Gib mir nur Daten aus diesem Bereich."

## Was map.on('moveend', function() {...}) wirklich bedeutet

Erstmal wichtig: Das hat nichts mit Ionic zu tun. Das ist reines Leaflet. map ist dein Karten-Objekt (das, was L.map('map') zurückgegeben hat), und .on ist eine Methode, die auf diesem Objekt existiert — genau wie .setView() oder .addTo().

Das Prinzip dahinter: "Event Listener"

Stell dir vor, die Karte macht ständig verschiedene Dinge: Sie wird gezoomt, verschoben, geklickt, usw. Jedes Mal, wenn so etwas passiert, sagt Leaflet intern "Achtung, gerade ist XYZ passiert" — das nennt man ein Ereignis (Event). moveend ist der Name für eines dieser Ereignisse: "Karte wurde fertig bewegt/gezoomt."

.on(...) heißt sinngemäß: "Wenn du das nächste Mal dieses Ereignis meldest, dann führe diese Funktion aus." Du meldest dich quasi als Zuhörer an ("Listener"), der benachrichtigt werden will.

Warum als String 'moveend'?

Leaflet hat vorab eine feste Liste an Ereignisnamen definiert, die es selbst auslöst — moveend, zoomend, click, dragend usw. Diese Namen sind einfach als Text (String) festgelegt, weil Leaflet intern irgendwo im Code sinngemäß macht: "Wenn sich die Karte gerade fertig bewegt hat, rufe alle Funktionen auf, die sich für 'moveend' angemeldet haben." Der String ist also wie eine Adresse/ein Etikett, unter dem sich deine Funktion einträgt.

Das ist kein Ionic- oder TypeScript-Spezialfall, sondern ein sehr verbreitetes Muster in JavaScript-Bibliotheken generell (du wirst das bei anderen Libraries in ähnlicher Form wiedersehen).
