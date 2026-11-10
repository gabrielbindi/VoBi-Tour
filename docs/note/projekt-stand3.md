# Projektstand

**Letztes Update:** 2026-11-10 13:41 CET
**Erstellt:** 2026-09-23 08:09 CEST

## Aktueller Stand (2026-11-10 13:41)

### Architektur / Übersicht

Diplomarbeit-App **VobiTour**: Constraint-basierte dynamische Tourenoptimierung für Liefer- und Entsorgungsfahrten. Ziel: effizienteste Rundtour (alle Punkte abfahren), Zeit sparen, unnötiges Hin-und-Her vermeiden.

**Zweiteilige Architektur (Client/Server):**

- **Client (Qt/QML):** UI + lokale Logik + Offline-Cache (SQLite). Zeigt Karte, Navigation, Stopp-Eingabe, Tourliste. Sync mit Server.
- **Server:** REST-API + Auth, Services, Routing-Engine (PostgreSQL + PostGIS + pgRouting + OR-Tools). Import von OSM-Daten.

Kommunikation: HTTP (lokal 127.0.0.1 oder Remote-IP). Server rechnet Touren, Client zeigt und navigiert.

**Server-Standort (final):**
- **Entweder** Mini-PC im Fahrzeug (Zigarettenanzünder-Ladung)
- **oder** Server in der Firma.
- Nicht beides gleichzeitig als Standard. Optional später: Priorität, falls beide vorhanden und erreichbar.

**Skalierung:**
- Typisch 100–400 Stopps möglich.
- Bei vielen Stopps: **zonale / hierarchische Optimierung** (System schlägt Zonen vor, Optimierung innerhalb der Zonen).

### Konfigurationen und Setups

- **UI-Framework:** Qt/QML
- **Server-Datenbank:** PostgreSQL + PostGIS + pgRouting
- **Client-Datenbank:** SQLite (Offline-Cache + lokale Änderungen)
- **Optimierer:** OR-Tools (TSP/VRP) für Stopp-Reihenfolge innerhalb der Zonen
- **Straßennetz:** OSM via osm2pgrouting (inkl. Oneways, maxheight/maxwidth/maxweight)
- **Traffic:** nur Anzeige / ETA, **nicht** für die Optimierung (Ziel = kürzeste Gesamtstrecke)
- **Stopp-Eingabe:** manuell, Bild/PDF (OCR), unterwegs durch Fahrer
- **Zonen:** System schlägt vor (PostGIS-Clustering + spätere natürliche Grenzen), Disponent kann anpassen

### Wichtige Entscheidungen

- **Produktname:** VobiTour (Vogrin + Binderbauer)
- **Team:** Mario Vogrin (mario-hvo), Gabriel Binderbauer
- **Titel:** VobiTour – Constraint-Based Dynamic Tour Optimization for Delivery and Disposal Operations
- **UI:** Qt/QML
- **Datenbank-Stack:** PostgreSQL + PostGIS + pgRouting (Server) + SQLite (Client) + OR-Tools
- **Routing-Workflow:**
  1. Import OSM + Parser für Constraints + Fahrer-/Disponenten-Korrekturen
  2. Zonen-Vorschlag (PostGIS)
  3. Matrix: `pgr_dijkstraCostMatrix` auf gefilterten Edges
  4. Sequence: OR-Tools
  5. Geometrie: `pgr_dijkstra` pro Teilstrecke
  6. Speicherung + Sync
- **Constraints:** Höhe, Breite, Gewicht als Edge-Filter. Bei fehlenden Werten oder großem Umweg → **Warnung** (nicht hart blockieren).
- **Oneways:** über `cost` / `reverse_cost`

**Rollen:**
- **System:** schlägt Zonen vor, rechnet Touren.
- **Disponent:** Supporter (nicht immer da). Kann Zonen anpassen, fehlende Daten eintragen, greift ein bei Kundenanrufen. Mehrere Disponenten betreuen zugeordnete Fahrer (nicht alle sehen alle).
- **Fahrer:** fährt die Tour. Darf Stopps hinzufügen, als erledigt markieren, Probleme melden.

**Offline-Regel (final):**
- Offline möglich: Tour anzeigen, Status ändern, Stopps manuell hinzufügen.
- Bei manuellem Stopp unterwegs entscheidet der Fahrer selbst: als Nächstes / an Position X / ans Ende.
- **Keine volle automatische Neuberechnung offline.**
- Volle Neuberechnung nur wenn der konfigurierte Server erreichbar ist.
- Bei Wiederverbindung: Sync der lokalen Änderungen; Server kann bei Bedarf neu rechnen.

**Abweichung / Recalculation:**
- Ab ca. 50 m: Nachfrage „Neu berechnen?“
- Ab ca. 150 m: stärkere Aufforderung
- Mit Throttling.

### Offene Punkte und nächste Schritte

- Parser + Validierung für OSM-Constraint-Werte (Einheiten, freie Texte)
- Tabelle für Korrekturen (Disponent/Fahrer), die Re-Imports überlebt
- Konkrete Zonen-Logik (einfaches Clustering zuerst, natürliche Grenzen später)
- Zuordnung Disponent ↔ Fahrer
- Qt-Projekt aufsetzen
- Vollständige Feature-Liste
- Sprache der Arbeit (DE/EN) bestätigen

### Relevante Dateien und Pfade

- `/workspace/artifacts/AGENTS.md` – ursprüngliche Projektvorgabe
- `/workspace/artifacts/projekt-stand.md` – dieser Snapshot
- Architektur-Diagramme (draw.io in VS Code Extension): 5 Seiten

### Änderungen seit letztem Stand

- **Server-Standort final:** entweder Mini-PC im Fahrzeug oder Server in der Firma (nicht beides standardmäßig).
- **Zonale Optimierung** als Lösung für 100–400 Stopps festgelegt.
- **Rollen klar definiert:** Disponent = Supporter, Fahrer hat unterwegs Kontrolle.
- **Offline-Regel final:** nur manuelles Einfügen von Stopps, keine volle Neuberechnung offline.
- Live-Traffic nur für Anzeige/ETA, nicht für Optimierung.
- Warnung statt hartem Blockieren bei unsicheren Constraints.
- Recalculation-Schwellen (50 m Nachfrage, 150 m stärker) festgelegt.

---

## Ältere Stände

## Aktueller Stand (2026-11-10 12:50)

### Architektur / Übersicht

Diplomarbeit-App **VobiTour**: Constraint-basierte dynamische Tourenoptimierung für Liefer- und Entsorgungsfahrten. Ziel: effizienteste Rundtour (alle Punkte abfahren), Zeit sparen, unnötiges Hin-und-Her vermeiden.

**Zweiteilige Architektur (Client/Server):**

- **Client (Qt/QML):** UI + lokale Logik + Offline-Cache. Zeigt Karte, Navigation, Stopp-Eingabe, Tourliste. Lokale DB: SQLite. Sync mit Server.
- **Server:** REST-API + Auth, Services (Vehicles, Tour, Stop, Sync, Corrections), Routing-Engine, Data-Layer (PostgreSQL + PostGIS + pgRouting). Import von OSM-Daten.

Kommunikation: HTTP (lokal 127.0.0.1 oder Remote-IP). Server rechnet Touren, Client zeigt und navigiert. Server kann lokal am Gerät oder ausgelagert laufen (IP umstellen).

**Architektur-Diagramme (5 Seiten):**
1. Client-Server-Übersicht
2. Client (Qt/QML) detailliert
3. Server detailliert
4. Routing-Engine
5. Input (neuer Stopp / Lieferschein)

### Konfigurationen und Setups

- **UI-Framework:** Qt/QML (entschieden nach Vergleich mit Ionic)
- **Server-Datenbank:** PostgreSQL + PostGIS + pgRouting
- **Client-Datenbank:** SQLite (Offline-Cache + Sync)
- **Optimierer:** OR-Tools (TSP/VRP) für Stopp-Reihenfolge
- **Straßennetz:** OSM via osm2pgrouting (inkl. Oneways, maxheight/maxwidth/maxweight)
- **Geocoding:** externer Geocoder
- **Traffic:** Traffic Data API (Einfluss auf Kosten noch offen)
- **Stopp-Eingabe:** manuell, Bild/PDF (OCR), später Dispatcher
- Serverstandort: flexibel (Default-Vorschlag: Laptop/Mini-PC im Fahrzeug oder Depot; Phone nur wenn Prototyp funktioniert)

### Wichtige Entscheidungen

- **Produktname:** VobiTour (Vogrin + Binderbauer)
- **Team:** Mario Vogrin (mario-hvo), Gabriel Binderbauer
- **Titel:** VobiTour – Constraint-Based Dynamic Tour Optimization for Delivery and Disposal Operations
- **UI:** Qt/QML
- **Datenbank-Stack (V2.0 entschieden):**
  - PostgreSQL + PostGIS + pgRouting auf dem Server
  - SQLite als Offline-Cache auf dem Client
  - OR-Tools für die Reihenfolge (TSP/VRP)
- **Warum PostgreSQL:** Alles in einer DB (Fahrer, Kunden, Fahrzeuge, Touren, Stopps, Straßennetz), PostGIS für räumliche Abfragen, pgRouting für echtes Straßennetz-Routing mit Oneways und Constraint-Filtern (Höhe/Breite/Gewicht), klassisch relational.
- **Warum nicht MongoDB:** Nur Luftlinien-Geo-Queries, kein echtes Routing über Straßennetz, SSPL-Lizenz.
- **Warum nicht Neo4j:** GDS kann gewichteten Dijkstra, aber Constraint-Filter pro Fahrzeug schwerer umsetzbar; nicht mobil verfügbar.
- **Routing-Workflow:**
  1. Import: OSM via osm2pgrouting + Parser für Höhe/Breite/Gewicht + Fahrer-Korrekturen
  2. Matrix: `pgr_dijkstraCostMatrix` auf gefilterten Edges (Fahrzeug-Constraints)
  3. Sequence: OR-Tools (asymmetrisch wegen Oneways)
  4. Geometrie: pro Teilstrecke `pgr_dijkstra` + Join mit `ways` für Linien + Warnungen
  5. Speicherung: Route mit Schlüssel (Stopps + Start + Fahrzeugprofil + Straßen-Datenversion)
  6. Offline: Client hält Tour + Route in SQLite und synct später
- **Constraints:** Höhe, Breite, Gewicht als Edge-Filter in pgRouting (`WHERE maxheight IS NULL OR maxheight >= ...`)
- **Oneways:** über `cost` / `reverse_cost` (negativ = nicht befahrbar)

### Offene Punkte und nächste Schritte

- **Serverstandort Default:** Prototyp nötig (Termux/Linux-VM auf Phone vs. Laptop/Mini-PC). Vorschlag: Default = Laptop/Mini-PC.
- Live-Traffic: beeinflusst Kosten oder nur Anzeige/ETA?
- Regel für Straßen ohne maxheight/maxwidth/maxweight (erlauben mit Warnung oder blocken?)
- Wie kommen height/width/weight-Tags sauber in die DB (custom osm2pgrouting mapconfig + Parser für freie Texte/Einheiten)?
- Tabelle für Fahrer-Korrekturen (OSM-way-id, Attribut, Wert, User, Datum) – nicht vom Re-Import überschreiben
- Cache-Invalidierung: Key = Stopps + Start + Fahrzeugprofil + Straßen-Datenversion
- Vollständige Feature-Liste (weiterhin offen)
- Qt-Projekt konkret aufsetzen (Karte, Navigation, Sync)
- Online-Bereitstellung
- Sprache der Arbeit (DE/EN) bestätigen

### Relevante Dateien und Pfade

- `/workspace/artifacts/AGENTS.md` – ursprüngliche Projektvorgabe
- `/workspace/artifacts/projekt-stand.md` – dieser Snapshot
- Architektur-Diagramme (draw.io): 5 Seiten (Client-Server, Client Qt/QML, Server, Routing-Engine, Stopp-Input)
- GitHub-Issue: DB-Evaluation (V2.0, mario-hvo) – Entscheidung PostgreSQL dokumentiert

### Änderungen seit letztem Stand

- **Datenbank-Entscheidung getroffen:** PostgreSQL + PostGIS + pgRouting (Server) + SQLite (Client) + OR-Tools.
- Routing-Workflow vollständig definiert (Matrix → OR-Tools → Geometrie).
- Architektur-Diagramme (5 Seiten) gelesen und in den Stand integriert.
- Offene Punkte aus der DB-Evaluation (Serverstandort, Traffic, fehlende Constraint-Werte, Parser, Korrekturen) übernommen.
- Oneway-Handling und asymmetrische Matrix dokumentiert.

---

## Aktueller Stand (2026-10-23 10:18)

### Architektur / Übersicht

Diplomarbeit-App **VobiTour**: berechnet den effizientesten Weg, um alle Punkte in einer Rundtour abzufahren. Ziel ist weniger Arbeitszeit und weniger unnötiges Hin-und-Her.

Zweiteilige Architektur:

- **Server:** macht die Tourenberechnung. Kann am Client mitlaufen oder ausgelagert werden (andere Maschine). Client stellt die Ziel-IP um, wenn das Handy zu schwach ist.
- **Client:** visuelle Karte und Navigation („wohin als Nächstes“). Zeigt geplante Route plus Straßenattribute.

Kommunikation: Server rechnet, schickt Ergebnis an die Client-App.

Anwendungsfälle laut Titel: **Lieferung und Entsorgung** (Delivery and Disposal Operations).

### Konfigurationen und Setups

- **UI-Framework: Qt/QML** (Entscheidung nach Technologie-Vergleich mit Ionic)
- Serverstandort: flexibel (lokal am Gerät oder remote per IP)
- App soll **online** verfügbar gemacht werden (geplant, noch nicht umgesetzt)
- Bisher kein Repo, keine konkreten Server-Ports, keine Karten-API und kein Routing-Algorithmus festgelegt

### Wichtige Entscheidungen

- **Produkt-/App-Name:** VobiTour (aus Vogrin + Binderbauer)
- **Team:** Mario Vogrin, Gabriel Binderbauer
- **Titel EN (festgelegt):** VobiTour – Constraint-Based Dynamic Tour Optimization for Delivery and Disposal Operations
- **Titel DE:** VobiTour – Constraint-basierte dynamische Tourenoptimierung für Liefer- und Entsorgungsfahrten
- Verworfene Namen: Bimbelrhauer; Dynamic Delivery and Garbage Routing („Garbage“ zu salopp)
- Kernfunktion der Optimierung: **Constraints** – Höhe, Breite, Gewicht
- Karte soll anzeigen:
  - Maximalgewichte
  - Höhen der Straßen / Durchfahrten
  - Breiten der Straßen
- Pflichtkontrollen: Höhenkontrolle, Breitenkontrolle, Gewichtskontrolle
- Client/Server-Trennung bewusst, damit schwere Geräte die Rechnung abgeben können
- **UI-Technologie: Qt/QML** (nach Vergleichsspike Ionic vs. Qt/QML)

### Offene Punkte und nächste Schritte

- Vollständige Feature-Liste aufschreiben (vom Team gewünscht, noch nicht gemacht)
- Routing-Verfahren festlegen (TSP / VRP / Heuristik, dynamisches Nachrechnen)
- Datenquelle für Straßenattribute (Höhe, Breite, Gewichtsbeschränkungen) klären
- Kartenanbieter / Kartentechnologie für Qt wählen (z.B. Qt Location, Mapbox, OSM)
- Server-Technik wählen (Sprache, API, Hosting)
- Qt-Projekt aufsetzen (Karte, Fahrzeugprofil, Tour, Constraints)
- Online-Bereitstellung (Hosting Client + optional Server)
- Scope schärfen: Lieferung und Entsorgung gleichwertig oder ein Hauptfall?
- Sprache der Arbeit / Einreichung (DE, EN oder beides) bei der Schule bestätigen

### Relevante Dateien und Pfade

- `/workspace/artifacts/AGENTS.md` – ursprüngliche Projektvorgabe
- `/workspace/artifacts/projekt-stand.md` – dieser Snapshot

### Änderungen seit letztem Stand

- Entscheidung für **Qt/QML** nach dem Technologie-Vergleich (Ionic vs. Qt) getroffen und festgehalten.
- UI-Framework von Ionic auf Qt/QML umgestellt.
- Offene Punkte entsprechend angepasst (Qt-Projekt statt Ionic-Projekt).

---

## Aktueller Stand (2026-09-23 08:09)

### Architektur / Übersicht

Diplomarbeit-App **VobiTour**: berechnet den effizientesten Weg, um alle Punkte in einer Rundtour abzufahren. Ziel ist weniger Arbeitszeit und weniger unnötiges Hin-und-Her.

Zweiteilige Architektur:

- **Server:** macht die Tourenberechnung. Kann am Client mitlaufen oder ausgelagert werden (andere Maschine). Client stellt die Ziel-IP um, wenn das Handy zu schwach ist.
- **Client:** visuelle Karte und Navigation („wohin als Nächstes“). Zeigt geplante Route plus Straßenattribute.

Kommunikation: Server rechnet, schickt Ergebnis an die Client-App.

Anwendungsfälle laut Titel: **Lieferung und Entsorgung** (Delivery and Disposal Operations).

### Konfigurationen und Setups

- UI-Framework: **Ionic**
  - https://ionicframework.com/
  - https://ionicframework.com/docs/components
- Serverstandort: flexibel (lokal am Gerät oder remote per IP)
- App soll **online** verfügbar gemacht werden (geplant, noch nicht umgesetzt)
- Bisher kein Repo, keine konkreten Server-Ports, keine Karten-API und kein Routing-Algorithmus festgelegt

### Wichtige Entscheidungen

- **Produkt-/App-Name:** VobiTour (aus Vogrin + Binderbauer)
- **Team:** Mario Vogrin, Gabriel Binderbauer
- **Titel EN (festgelegt):** VobiTour – Constraint-Based Dynamic Tour Optimization for Delivery and Disposal Operations
- **Titel DE:** VobiTour – Constraint-basierte dynamische Tourenoptimierung für Liefer- und Entsorgungsfahrten
- Verworfene Namen: Bimbelrhauer; Dynamic Delivery and Garbage Routing („Garbage“ zu salopp)
- Kernfunktion der Optimierung: **Constraints** – Höhe, Breite, Gewicht
- Karte soll anzeigen:
  - Maximalgewichte
  - Höhen der Straßen / Durchfahrten
  - Breiten der Straßen
- Pflichtkontrollen: Höhenkontrolle, Breitenkontrolle, Gewichtskontrolle
- Client/Server-Trennung bewusst, damit schwere Geräte die Rechnung abgeben können

### Offene Punkte und nächste Schritte

- Vollständige Feature-Liste aufschreiben (vom Team gewünscht, noch nicht gemacht)
- Routing-Verfahren festlegen (TSP / VRP / Heuristik, dynamisches Nachrechnen)
- Datenquelle für Straßenattribute (Höhe, Breite, Gewichtsbeschränkungen) klären
- Kartenanbieter wählen (OSM, Google, Mapbox, …)
- Server-Technik wählen (Sprache, API, Hosting)
- Ionic-Projekt aufsetzen (Tabs, Karte, Fahrzeugprofil, Tour)
- Online-Bereitstellung (Hosting Client + optional Server)
- Scope schärfen: Lieferung und Entsorgung gleichwertig oder ein Hauptfall?
- Sprache der Arbeit / Einreichung (DE, EN oder beides) bei der Schule bestätigen

### Relevante Dateien und Pfade

- `/home/workdir/artifacts/AGENTS.md` – ursprüngliche Projektvorgabe (Ionic, Server/Client, Constraints, Features aufschreiben, App online)
- `/home/workdir/artifacts/projekt-stand.md` – dieser Snapshot

### Änderungen seit letztem Stand

- Erster Snapshot. Kein älterer Stand vorhanden.
- Titel und App-Name festgehalten.
- Teamnamen festgehalten.
- Architektur und Pflichtfeatures aus der Vorgabe übernommen.
