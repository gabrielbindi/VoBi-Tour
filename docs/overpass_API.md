## Schritt 1: Die Overpass-Abfrage verstehen (ohne Code)

Die **Overpass API** ist eine Datenbank-Abfragesprache für OpenStreetMap-Daten. Du schickst ihr eine Textanfrage ("gib mir alle Haltestellen in diesem Bereich") und bekommst JSON zurück.

Teste die Abfrage zuerst **im Browser**, ohne Code, damit du siehst, was zurückkommt, bevor wir das in Ionic einbauen:

👉 https://overpass-turbo.eu/

Dort rechts im Editor diese Abfrage einfügen (das ist "Overpass QL", die Abfragesprache):

```
[out:json];
node["public_transport"="platform"](47.02,15.30,47.12,15.55);
out;
```

## Was diese Abfrage bedeutet

**`[out:json]`**: Antwortformat soll JSON sein (statt dem Standard-XML).

**`node[...]`**: Suche nach Punkten (nicht Linien oder Flächen) mit einer bestimmten Eigenschaft.

**`["public_transport"="platform"]`**: Nur Punkte, die in OSM als `public_transport=platform` markiert sind — das ist der Standard-Tag für Haltestellen (Bus, Tram usw.).

**`(47.02,15.30,47.12,15.55)`**: Die Bounding Box — Südwest-Breite, Südwest-Länge, Nordost-Breite, Nordost-Länge. Das ist grob der Bereich von Graz. Erkennst du das Muster? Genau das Gleiche, was du schon mit `map.getBounds()` ausliest!

**`out;`**: "Gib die Ergebnisse aus."
