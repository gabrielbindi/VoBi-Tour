# Projektstand

**Letztes Update:** 2026-11-10 14:47 CET
**Erstellt:** 2026-09-23 08:09 CEST

## Aktueller Stand (2026-11-10 14:47)

### Architektur / Übersicht

Diplomarbeit-App **VobiTour**: Constraint-basierte dynamische Tourenoptimierung für Liefer- und Entsorgungsfahrten.

**Client/Server:**
- Client (Qt/QML): UI, lokale Logik, SQLite-Cache, Sync.
- Server: REST-API, PostgreSQL + PostGIS + pgRouting + OR-Tools.

**Server-Standort:** Entweder Mini-PC im Fahrzeug oder Server in der Firma (nicht beides standardmäßig).

**Skalierung:** Zonale Optimierung für 100–400 Stopps. System schlägt Zonen vor, Disponent kann anpassen/löschen/neu zeichnen.

### Konfigurationen und Setups

- UI: Qt/QML
- Server-DB: PostgreSQL + PostGIS + pgRouting
- Client-DB: SQLite
- Optimierer: OR-Tools innerhalb der Zonen
- Straßennetz: OSM (osm2pgrouting) + Parser für Constraints
- Traffic: nur Anzeige/ETA, nicht für Optimierung
- Zonen: PostGIS-Clustering + Regeln für lange Straßen/Sackgassen

### Wichtige Entscheidungen

- **Zonen-Regeln:**
  - Klein halten (besonders am Land).
  - Lange Straßen/Sackgassen → eigene Zonen oder klare Abschnitte (Anfang/Ende).
  - Bevorzugt eine Straßenseite komplett, dann die andere.
  - Seitenwechsel nur bei sicheren Bedingungen (≤ 50 km/h, nicht zu breit, und nicht wenn man sowieso zurückfährt).
  - Kleine Abzweigungen/Dörfer: beide Varianten rechnen + Differenz anzeigen + Empfehlung („besser jetzt mitnehmen“).
- **Sicherheit & Empfehlungen:**
  - System schlägt vor (automatisch aus OSM).
  - Fahrer oder Disponent können überschreiben.
  - Jede Änderung wird protokolliert und erscheint im Tour-Bericht.
  - OSM-Originalwerte immer als Hinweis anzeigen.
  - Fahrer kann Höhe/Breite/Gewicht während der Tour korrigieren (besonders bei fehlenden/falschen OSM-Daten).
- **Offline:** Nur manuelles Einfügen von Stopps (Fahrer wählt Position). Keine volle Neuberechnung offline.
- **Rollen:** Disponent = Supporter (kann Zonen anpassen). Fahrer fährt und kann unterwegs ändern. Mehrere Disponenten ↔ zugeordnete Fahrer.
- **Änderungen:** Grober Richtwert sofort, echte Neuberechnung auf Knopfdruck. Abweichung zur besten Route immer anzeigen.
- **Nice-to-have:** Lernendes System (merkt sich Fahrer-/Disponenten-Entscheidungen für ähnliche Straßen).

### Offene Punkte und nächste Schritte

- Konkrete Schwellenwerte für Zonengröße am Land erforschen.
- Parser + Validierung OSM-Constraint-Werte.
- Report-Tool für Tour-Berichte.
- Zuordnung Disponent ↔ Fahrer.
- Qt-Projekt + Feature-Liste.

### Relevante Dateien und Pfade

- `/workspace/artifacts/projekt-stand.md`
- `/workspace/artifacts/issues.md` – kompakte Git-Issues aus den Erkenntnissen
- Architektur-Diagramme (draw.io): noch weitgehend passend, Zonen-Logik und Offline-Regeln fehlen noch explizit

### Änderungen seit letztem Stand

- Detaillierte Zonen-Regeln (Seiten, Sicherheit, Sackgassen, kleine Dörfer) festgelegt.
- Empfehlungssystem + Tour-Bericht + Protokollierung.
- Fahrer kann Constraints während der Tour korrigieren; OSM-Original immer sichtbar.
- Lernendes Verhalten als Nice-to-have.
- Issues-Datei erstellt.

---

## Ältere Stände

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

## Aktueller Stand (2026-11-10 12:50)

[... älterer Inhalt bleibt erhalten ...]
