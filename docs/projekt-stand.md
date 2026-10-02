# Projektstand

**Letztes Update:** 2026-09-23 08:09 CEST
**Erstellt:** 2026-09-23 08:09 CEST

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

---

## Ältere Stände

(noch keine)
