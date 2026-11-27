# Projektstand

**Letztes Update:** 2026-11-27 23:25 CET
**Erstellt:** 2026-09-23 08:09 CEST

## Aktueller Stand (2026-11-27 23:25)

Dieser Stand sammelt alles, was in die Doku der Diplomarbeit gehört: Architektur, Abläufe, Regeln, Datenmodell, Routing, bekannte Grenzen und die Ergebnisse der kritischen Prüfung. Das finale ERD ist in draw.io gezeichnet (`erd.png`, Legende: blau = Stammdaten, grün = Tagesbetrieb, orange = Routing eigene Tabellen, braun = OSM-Import nicht migriert; gestrichelt = logisch über OSM-ID, kein FK). Als Vorlage diente das Artifact „VobiTour ERD". **Stand: bereit für die Migration (#21).**

### 1. Überblick

Diplomarbeit-App **VobiTour** – Constraint-basierte dynamische Tourenoptimierung für Liefer- und Entsorgungsfahrten (generalisierbar für Post, Müllabfuhr, Ambulanz).

**Team:** Mario Vogrin (Backend), Gabriel Binderbauer (Frontend Qt/QML)

**Kern:** Die App berechnet die schnellste Runde über alle Stops und beachtet dabei Höhe, Breite und Gewicht des Fahrzeugs. Hauptfall sind Straßen, die abgefahren werden (Müllabfuhr) → **Arc Routing (CARP)**. Wo es nur um einzelne Punkte geht (Ambulanz), bleibt es einfaches Punkt-Routing.

### 2. Architektur

**Version 1 (Ziel der Diplomarbeit, „Variante C"):**
- Jeder Fahrer arbeitet eigenständig. Server + PostgreSQL laufen im Fahrzeug (kleiner PC), der Client (Qt/QML) läuft am Handy/Tablet.
- Die Datenbank lebt im Fahrzeug und bleibt dort, inklusive der Korrekturen der Fahrer. Neue Stammdaten (Fahrer, Accounts, Fahrzeuge, Kunden, Adressen) kommen per Seed-Skript aus der Master-Kopie (z. B. Repo) dazu, **ohne lokale Korrekturen zu überschreiben** (bestehende Adressen werden nicht verändert). Danach fährt jedes Fahrzeug selbständig.
- Am Ende jeder Tour wird ein Bericht ausgegeben. Er ist die Historie.
- Übergaben von Stops zwischen Fahrern machen die Fahrer selbst.

**Erweiterung (nur auf Wunsch einer Firma):**
- Disponenten-Client (Web-App oder CLI) und eine zentrale Master-DB, die vor Tourbeginn aktualisiert und in die Fahrzeuge kopiert wird.
- Historie aus den Tourberichten in der Disponenten-Ansicht.
- Das Datenmodell bleibt gleich. Neu ist nur `created_by = dispatcher`.
- Erst dann nötig: eindeutige IDs über alle Fahrzeuge (UUID statt `int` für Tabellen, die im Fahrzeug entstehen).

**Rollen:**
- **Fahrer-Client:** Tour, Stopps, Scan, Profil, Problem melden/lösen, Zonen bearbeiten.
- **Disponent-Client (optional):** Zonen und Stops anlegen, Stops von Fahrer A zu Fahrer B übergeben.
- **Server:** REST-API, PostgreSQL + PostGIS + pgRouting, OR-Tools (Python).
- Kommunikation HTTP/JSON, Server-IP im Client umstellbar (127.0.0.1 oder andere IP).

**Begriffe sauber trennen:**
- **Stop-Snapping** (Server): legt eine Adresse auf einen Straßenabschnitt und bestimmt die Straßenseite.
- **GPS-Snapping** (Client): legt die aktuelle GPS-Position auf die Route während der Fahrt.

**Datenbanken:**
- Das ERD zeigt die **Server-DB** (PostgreSQL).
- Die **Client-DB** (SQLite: Cache + Outbox für Offline) ist ein eigenes, kleines Schema und wird separat dokumentiert.

### 3. Fahrer-Client (Version 1)

| Bereich | Funktionen | Tabellen |
|---|---|---|
| Tour | Route, nächster Stop, gemiedene Durchfahrten, Warnungen | Tour, Stop |
| Stopps | verschieben (Pin), hinzufügen, Depot-Rückkehr, fertig / `reassigned`, Problem melden und lösen | Stop, Location, Stop_Crew, Problem, Photo |
| Scan | neuer Stop aus Lieferschein (Foto, PDF) oder manuell | Delivery_note → Stop → Location |
| Profil | Fahrzeugmaße | Vehicle |
| Zonen | Vorschlag ansehen, ändern, neu zeichnen, Startpunkt setzen | Zone |

### 4. Abläufe und Regeln

#### 4.1 Tagesablauf (Version 1)
1. Anmelden (Account). Erst danach sieht der Fahrer seine Tour (über `Account.crew_id` → `Tour_Crew`).
2. Stammdaten werden **nicht automatisch** aktualisiert. Gibt es etwas Wichtiges Neues (z. B. neuer Fahrer, neue Adressen), zeigt die App das an und fragt, ob es eingespielt werden soll. Eingespielt wird nur hinzugefügt, bestehende Adressen und Korrekturen bleiben.
3. Stops kommen per Lieferschein-Scan, PDF oder manuell dazu.
4. Server bildet ggf. Zonen und rechnet die Route.
5. Fahrer fährt, hakt ab, meldet Probleme. Neue Stops lösen eine Neuberechnung aus (nur die betroffene Zone).
6. Am Tourende: Bericht ausgeben, inklusive aller Korrekturen (verschobene Pins, Notizen, Adress-Fotos, Straßenregeln).

#### 4.2 Stops
- Jeder Stop gehört zu genau einer Tour (`tour_id` Pflicht).
- Ein Stop ist ein **Besuch** einer Adresse. Die Adresse (`Location`) wird über viele Touren wiederverwendet.
- `status`:
  - `open` – noch nicht erledigt
  - `done` – angefahren und erledigt (es gibt eine `Stop_Crew`-Zeile mit Zeiten)
  - `skipped` – vom Fahrer als erledigt markiert, ohne dort zu halten (z. B. schon erledigt, nicht nötig). Es gibt trotzdem eine `Stop_Crew`-Zeile, `arrived_at` und `finished_at` haben dieselbe Zeit (Zeitpunkt des Abhakens).
  - `reassigned` – an einen anderen Fahrer übergeben
- `Stop.status` ist die Wahrheit für die App, `Stop_Crew` ist das Protokoll.
- **Spezial-Stops** (`special_type`):
  - `start` – Beginn der Tour, angelegt aus der Depot-Adresse. Genau einer pro Tour.
  - `depot` – Zwischenstopp zum Ausleeren / Nachladen, mit `priority`. Beliebig viele.
  - `end` – Ende der Tour, normalerweise zurück zum Depot. Höchstens einer; fehlt er, gilt Ende = Start.
- **Eine Tour bedient nur eine Art** (`Tour.cargo_type`, z. B. Tour „Papier Gleisdorf"). Mögliche Werte z. B. `paper`, `residual_waste`, `packaging`, `parcel`, `passenger` (Personentransport/Ambulanz). An einer Adresse werden nur die Behälter mit diesem Typ abgeholt. `Tour.counter` zählt allgemein (z. B. gelbe Säcke).
- `is_special` wurde gestrichen: Ein Stop ist speziell, wenn `special_type` gefüllt ist.

#### 4.3 Wer hat den Stop gemacht (`Stop_Crew`)
- Eine Zeile entsteht beim **tatsächlichen Halt**: `crew_id` = wer dort war, `arrived_at`, `finished_at` beim Wegfahren.
- Mehrere Zeilen pro Stop sind erlaubt (zweiter Anfahrtsversuch).
- `skipped`: Zeile mit `arrived_at` = `finished_at` (Zeitpunkt des Abhakens).
- Übergabe-Vermerk: Zeile ohne Zeiten (siehe 4.4).
- Sonst gilt: kein Halt → keine Zeile.

#### 4.4 Übergabe von Stops (Einspringen)
- Disponent (oder in Version 1 der Fahrer selbst) gibt an: von Fahrer A zu Fahrer B.
- **Bei A:** Stop auf `reassigned`, eine `Stop_Crew`-Zeile mit `crew_id` = B und `originally_assigned_to` = A, **ohne Zeiten** (= Übergabe-Vermerk, keine Fahrt). Route wird neu berechnet.
- **Bei B:** die Stops werden ganz normal neu angelegt (wie manuell hinzugefügt) in B's eigener Tour, `originally_assigned_to` leer. Route wird neu berechnet.
- Damit das funktioniert, muss B in A's Datenbank existieren: Crew-Liste ist Teil der kopierten Master-DB, `service_number` ist eindeutig.
- Bekannte Grenze: Ist ein Fahrzeug offline, kommt die Übergabe nicht an → Fahrer hakt selbst ab.

#### 4.5 Adressen, Pins und Hauseinfahrten
- Der Fahrer kann den Pin (`Location.geom`) verschieben, damit die Markierung genauer wird. Die Adresse (Straße, Hausnummer, PLZ, Ort) bleibt gleich.
- **Lange Einfahrt, nicht in OSM:** Pin an die Einfahrt des Anwesens setzen, dort hält das Fahrzeug. Wo die Tonne steht, kommt in `Location.note` und als Adress-Foto.
- **Privater Weg:** Es wird nicht davon ausgegangen, dass Post, Müllabfuhr oder Lieferant dort fahren darf.
- **Kunde möchte den Stop an der öffentlichen Straße:** Pin an die Straße setzen, Adresse bleibt gleich.

#### 4.6 Probleme und Fotos
- Ein Problem gehört zu genau einem Stop, ein Stop kann keines oder mehrere haben. Gelöst = `resolved_at` gesetzt.
- Jedes Foto gehört **eindeutig zu einem Stop** (`stop_id` Pflicht), optional zu einem Problem.
- Fotos, die für die Adresse wichtig sind (z. B. Einfahrt, Standort der Tonne), bekommen `is_location_info = true`. Sie werden bei jeder späteren Tour an dieser Adresse angezeigt (über `Stop.location_id`). Das funktioniert, weil die Fahrzeug-DB nicht überschrieben wird.

#### 4.7 Fehlende Werte und Warnungen
- Fehlt in OSM Höhe, Breite oder Gewicht, gilt die Straße als **befahrbar**, der Client bekommt eine **Warnung**, der Fahrer entscheidet.
- Führt die Route über einen privaten Weg → ebenfalls Warnung.
- **Unterführungen sind am wichtigsten:** Führt ein Abschnitt unter einer Brücke durch oder durch einen Tunnel (`Street_attribute.passes_under`) und ist keine Höhe bekannt → Warnung mit Dringlichkeit `critical`.
- **Ablauf:** Parser erkennt die Unterführung → Warnung → Fahrer prüft vor Ort und trägt die erlaubte Höhe für diesen Abschnitt in `Street_override.max_height` ein (egal ob Brücke oder nicht) → Warnung verschwindet. Hat sich der Parser geirrt, korrigiert `Street_override.passes_under` das.
- Warnungen gibt es auf zwei Ebenen: **pro Stop** (`Stop.warning`, z. B. nicht erreichbar) und **pro Straßenabschnitt** (`Route_warning` mit `osm_way_id`, `kind`, `severity`).

#### 4.8 Die fertige Tour
Eine eigene Tabelle „fertige Tour" braucht es nicht, sie setzt sich zusammen aus:
- **Geplant:** Reihenfolge `Stop.sequence`, geplante Ankunft `Stop.arrival`, Linie, Länge und Fahrzeit in der aktiven `Route`.
- **Tatsächlich:** wann wer wo war in `Stop_Crew` (die tatsächliche Reihenfolge ergibt sich aus `arrived_at`), Status je Stop, Beginn und Ende in `Tour_Crew`, `Tour.status` = fertig.
- **Änderungen** unterwegs im `Change_log`, Probleme und Fotos an den Stops.
- Daraus wird am Tourende der Bericht erzeugt.
- Nicht gespeichert: die tatsächlich gefahrene GPS-Spur. Falls ihr „geplant gegen gefahren" vergleichen wollt, kann sie später als weitere `Route`-Variante (`driven`) dazukommen (Nice to have).

### 5. Zonen

**Zweck:** Viele Stops (Ziel: 400) schnell rechnen, indem das Problem in Teile zerlegt wird.

- **Nur wenn sinnvoll:** Unter einer Grenze (z. B. 100 Stops) gibt es keine Zonen, alles wird in einem Lauf gerechnet.
- **Grenzen:** maximale Stop-Anzahl und maximale Ausdehnung pro Zone.
- **Ein Straßenabschnitt liegt immer in genau einer Zone.** Gemeint ist der Abschnitt von Kreuzung zu Kreuzung, nicht die ganze Straße. Deshalb wird nie nach Einzelpunkten geteilt, sondern immer nach „Abschnitt + Seite".
- **Arten (`Zone.kind`):**
  - `boundary` – Fläche zwischen Grenzstraßen. Grenzstraßen sind die `direction_only`-Abschnitte (schnell oder breit). Flächen entstehen mit `ST_Polygonize` aus diesen Linien und dem Gebietsrand. Stops an einer Grenzstraße gehören zur Zone auf ihrer Seite.
  - `cluster` – Stops, die innerhalb einer Reichweite liegen (`ST_ClusterDBSCAN`).
  - `split` – zu große Zone wird halbiert (`ST_ClusterKMeans`, k = 2), so oft wie nötig.
  - `corridor` – offene Grenzstraße mit genug Stops wird eigene Zone (schmaler Streifen um die Straße, `entry_point` an einem Ende; bei Einbahn fest, sonst wählt es die Zonenreihenfolge).
- **Mini-Zonen vermeiden:** Hat eine Korridor-Zone nur 1–2 Stops, kommen sie zur Nachbarzone auf ihrer Seite.
- **Ausreißer:** Ein Stop weit weg von allen anderen bekommt keine Zone (`zone_id = NULL`) und `side = any`. Er wird als einzelner Punkt zwischen den Zonen eingeplant (`ST_ClusterDBSCAN` ordnet solche Punkte keinem Cluster zu).
- **Zwischen Zonen** kann nichts liegen oder nur einzelne Ausreißer-Punkte, die trotzdem eine Richtung haben (siehe Straßenseiten).
- **Bearbeiten:** Das System schlägt vor, der Fahrer (später der Disponent) kann Zonen ändern, neu zeichnen, Stops darin anlegen und den Startpunkt setzen.
- **Gleiche Zone heißt nicht erreichbar.** Die Zone ist nur eine Fläche, gefahren wird auf dem Straßennetz. Die Route darf die Zonenfläche verlassen (Routing mit Puffer um die Zone).
- **Nice to have (später):** Grenzen aus Gemeindegrenzen (`boundary=administrative`, Gemeinden in AT `admin_level=8`), Flüssen (`waterway`) und Bahnlinien (`railway=rail`). Diese importiert osm2pgrouting nicht → eigener Import (z. B. Geofabrik-Auszug, `osmium`, `osm2pgsql`) in eine Tabelle `Zone_boundary`.

### 6. Straßen und Straßenseiten (Arc Routing)

- Ein **Abschnitt** ist eine Straße von Kreuzung zu Kreuzung, eindeutig über `osm_id + source_osm + target_osm`.
- **`serve_mode` pro Abschnitt:**
  - `both` – beide Seiten in einem Durchgang (Crew geht über die Straße).
  - `direction_only` – nur die Seite in Fahrtrichtung (bei Rechtsverkehr rechts). Die andere Seite kommt beim Zurückfahren dran.
- **Standardregel (ohne Eintrag):** langsam **und** schmal bzw. Wohnstraße → `both`, sonst oder unbekannt → `direction_only`. Sicherheit geht vor. Genaue Werte in Abschnitt 7a.
- `Street_override` speichert **eigene Korrekturen** pro Abschnitt (vom Fahrer, später vom Disponenten): abweichende Seitenregel und fehlende oder falsche Straßenwerte. Wirksam ist immer zuerst die eigene Korrektur, dann der OSM-Wert (`COALESCE`).
- **Seite pro Stop** (`Stop.side`: `left`, `right`, `any`) muss vor der Berechnung feststehen. Stop-Snapping berechnet sie aus der Lage zur Kante, der Fahrer kann `any` setzen.
- OR-Tools kennt keine Seiten. Die Matrix erledigt das: Ein Stop auf `direction_only` ist nur aus seiner Richtung erreichbar. Liegen Stops auf beiden Seiten, plant OR-Tools von selbst zwei Durchfahrten (erst vorbeifahren, auf dem Rückweg halten). Bei `both` zählt die kürzere Richtung.
- Beispiel: Stop auf einer schnellen Straße zwischen zwei Orten → nur auf der passenden Fahrt (hin oder zurück) erreichbar, OR-Tools ordnet ihn automatisch richtig ein.

### 7. Berechnung

1. **Kanten-Abfrage** (am besten als View `routing_edges`): verbindet `ways` mit `Street_attribute`, `Street_override` (eigener Wert vor OSM-Wert) und den Fahrzeugmaßen. Gesperrte Kanten bekommen Kosten `-1` (pgRouting ignoriert sie). Private Wege bekommen einen Strafaufschlag (z. B. × 10), damit sie nur genommen werden, wenn es keinen anderen Weg gibt.
2. **Kosten:** für die schnellste Runde `cost_s` / `reverse_cost_s` (Sekunden). Achtung: `cost` / `reverse_cost` sind bei osm2pgrouting die **Länge in Grad**, keine Geschwindigkeit und keine Zeit. Die Fahrzeit rechnet osm2pgrouting beim Import selbst aus Länge und Tempo (`maxspeed_forward` / `maxspeed_backward` in km/h aus OSM, fehlt es, Standardwert je Straßenklasse). Dafür braucht es **keine andere API**. Eine externe API (z. B. TomTom) nur für Live-Verkehr in der Anzeige.
3. **Matrix pro Zone:** `pgr_dijkstraCostMatrix` (Dijkstra ist exakt) mit gerichteten Kanten. Weil Stops mitten auf einem Abschnitt liegen, passt `pgr_withPointsCostMatrix` besser: Es nimmt Punkte mit Abschnitt (`edge_id`), Position (`fraction`) und Seite (`side` = `l`, `r`, `b`) und beachtet die Fahrtrichtung (`driving_side`). Das entspricht genau `Location.edge_fraction` und `Stop.side`. Die `edge_id` (= aktuelle `gid`) wird beim Rechnen über die OSM-IDs ermittelt. Verfügbarkeit in der verwendeten pgRouting-Version prüfen.
4. **Knoten zusammenfassen:** alle Stops auf demselben Abschnitt und derselben Seite = ein Knoten. Spart viel Rechenzeit.
5. **Zonenreihenfolge** zuerst (über die `entry_point`s, Ausreißer als einzelne Knoten), dann jede Zone mit **festem Start** (eigener `entry_point`) und **festem Ende** (Richtung nächste Zone).
6. **Neuberechnung:** Ändert sich ein Stop, wird nur dessen Zone neu gerechnet.
7. **Qualität:** OR-Tools liefert „nahezu optimal" im Zeitlimit. Bis ca. 15–20 Stops wäre exakt möglich (Held-Karp). Zonen kosten an den Grenzen etwas Qualität → im Kerneltest messen.
8. **Erreichbarkeit:** Fehlt ein Paar in der Matrix, gibt es mit diesem Fahrzeug keinen Weg → Warnung. Für OR-Tools fehlende Paare mit sehr hohem Wert füllen oder Stop weglassen, bis der Fahrer entschieden hat.
9. **Zwei Routen (wenn schnell genug, immer):**
   - **mit** allen Beschränkungen → wird angeboten.
   - **ohne** Straßenattribute (nur schnellste Runde nach Strecke und km/h) → Vergleich und Ursachenfindung („Stop 12 nur über Unterführung 3,2 m erreichbar, Fahrzeug 3,6 m").
   - Ist ein Stop **nur** ohne Beschränkungen erreichbar → Warnung mit Grund, der Fahrer entscheidet selbst, ob er diese Route nimmt.
   - Die Route ohne Beschränkungen wird **nie automatisch** zur Navigation.
   - Für die Arbeit: Umweg durch Beschränkungen messen (z. B. „+2,4 km, +6 min").
10. **Live-Traffic** nur für Anzeige/ETA, nicht für die Optimierung.
11. **Fahrzeug bei mehreren Zuweisungen:** für die Beschränkungen zählt das Fahrzeug mit dem neuesten `Tour_Vehicle.assigned_at`.
12. **Ergebnis speichern:** Reihenfolge und geplante Ankunft in `Stop.sequence` / `Stop.arrival`, die Linie und Kennzahlen pro Variante in `Route`, Warnungen zu Straßen in `Route_warning`, Warnungen zu Stops in `Stop.warning`.
13. **Neuberechnung unterwegs:** Startpunkt ist die aktuelle GPS-Position. Sie kommt vom Client mit der Anfrage und wird nicht gespeichert.
14. **Bewusst nicht gespeichert:** die Matrix (wird jedes Mal neu gerechnet).

### 7a. Festgelegte Grenzwerte (Startwerte, im Kerneltest prüfen)

| Wert | Festlegung |
|---|---|
| Zonen bilden ab | mehr als 120 Knoten (Knoten = Stops nach Zusammenfassen pro Abschnitt + Seite) |
| max. Knoten pro Zone | 80 |
| max. Ausdehnung pro Zone | 3 km Durchmesser |
| Mindestgröße Korridor-Zone | 3 Stops, sonst zur Nachbarzone |
| Ausreißer / Cluster-Reichweite | 500 m (`ST_ClusterDBSCAN` mit eps 500 m, mind. 3 Stops pro Cluster) |
| Standardregel `both` | Tempo ≤ 50 km/h **und** (Breite ≤ 6 m **oder** ≤ 2 Fahrstreifen **oder** Straßenklasse `residential`, `living_street`, `service`, `unclassified`) |
| sonst / unbekannt | `direction_only` |
| Strafaufschlag private Wege | Kosten × 10 |
| Stop-Snapping max. Abstand | 100 m, sonst `way_osm_id` leer + Warnung |
| Routing-Puffer um eine Zone | 1 km |
| Zeitlimit OR-Tools | 10 s pro Zone, 60 s für die ganze Tour (400 Stops), 5 s für Neuberechnung einer Zone |
| fehlendes Matrix-Paar | Wert 1 000 000 s + Warnung |

Begründung Standardregel: Wohnstraßen haben in OSM selten eine Breite oder Fahrstreifen eingetragen. Ohne die Straßenklasse würden fast alle Wohnstraßen als `direction_only` gelten, was für die Müllabfuhr falsch ist.

### 8. OSM-Daten und Import

- **OSM-IDs** vergibt OpenStreetMap. Sie bleiben gleich, wenn eine Straße bearbeitet wird. Wird eine Straße geteilt oder neu gezeichnet, gibt es (teilweise) neue IDs.
- **`gid`, `source`, `target`** vergibt osm2pgrouting beim Import fortlaufend → ändern sich bei jedem Import. Eigene Tabellen verweisen deshalb nie auf `gid`, sondern auf OSM-IDs (logische Verknüpfung, kein FK).
- `ways` und `ways_vertices_pgr` legt osm2pgrouting selbst an → **nicht** in die eigenen Migrationen.
- `Street_attribute` wird bei jedem Import geleert und aus derselben OSM-Datei neu gefüllt (`TRUNCATE` + `INSERT`, am besten in einer Transaktion mit dem Import). Eigene Einträge gehören in `Street_override`.
- **Eigener Parser** füllt `Street_attribute` pro Abschnitt aus OSM-Tags: `maxheight`, `maxwidth`, `maxweight`, `width`, `lanes`, `access`. Achtung: Höhenbeschränkungen stehen in OSM teils an Punkten (z. B. Höhenbegrenzer) statt an der Straße → Parser prüft auch Knoten.
- `maxwidth` (erlaubte Fahrzeugbreite) ≠ `width` (Straßenbreite).
- **Unterführungen erkennen:** In OSM hat die Straße unter einer Brücke selbst meist keinen Hinweis darauf. Die Brücke ist ein eigener Weg mit `bridge=yes` und höherem `layer` (Straßen- oder Bahnbrücke). Der Parser holt alle Brücken und markiert jeden Straßenabschnitt, der eine Brücke mit höherem `layer` kreuzt (`ST_Intersects`), sowie Abschnitte mit `tunnel=yes` oder `covered=yes` als `passes_under = true`. Bahnbrücken importiert osm2pgrouting nicht, deshalb muss der Parser sie selbst lesen.
- `maxspeed_forward` / `maxspeed_backward` (km/h) liefert osm2pgrouting in `ways`.
- **Alle Wege importieren**, auch private. Gesperrt wird nicht beim Import, sondern über den Strafaufschlag beim Rechnen.
- **Nach jedem Import:**
  1. Alle `Location`s neu snappen.
  2. `Street_override`-Einträge ohne passenden Abschnitt auflisten (z. B. neue Kreuzung in OSM), jemand prüft sie.

### 9. Datenmodell (Server-DB)

**Gruppe Betrieb – Stammdaten**
- **Crew_member** – Personal. `service_number` eindeutig (gleicher Fahrer in jeder Fahrzeug-DB).
- **Account** – Login. Ohne Anmeldung keine Tour. Über `crew_id` sieht der Fahrer seine eigene Tour. `username` eindeutig, `password_hash` (nie das Passwort selbst speichern), `role` (`driver`, `dispatcher`, `admin`), `crew_id` verknüpft mit dem Fahrer (leer z. B. beim Disponenten). Pro Fahrer höchstens ein Account.
- **Vehicle** – Fahrzeug. `height`, `width` in m, `weight` in t (= zulässiges Gesamtgewicht, weil ein Müllwagen während der Tour schwerer wird).
- **Client** – Kunde (Name, Telefon).
- **Cargo** – Behälter/Ladung an einer Adresse, bezahlt von einem Kunden. `cargo_type`, `volume`.
- **Location** – Adresse. `is_depot` = Depot, aus dem `start`-, `depot`- und `end`-Stops angelegt werden. `geom` = Pin (vom Fahrer verschiebbar), `note` = Hinweis des Fahrers. Snap-Ergebnis: `way_osm_id`, `from_node_osm_id`, `to_node_osm_id` (welcher Abschnitt), `edge_fraction` (0 = Anfang, 1 = Ende). Snap-Felder dürfen leer sein.

**Gruppe Betrieb – Tagesbetrieb**
- **Tour** – Tagesrunde. `cargo_type` = welche Art diese Tour abholt/liefert (genau eine). `counter` = allgemeiner Zähler (z. B. gelbe Säcke). Die berechnete Route steht in `Route`.
- **Tour_Crew** – wer die Tour fährt (`crew_id`), optionaler Beifahrer (`co_driver_id`, nur einer), `started_at` / `finished_at`.
- **Tour_Vehicle** – welches Fahrzeug, `assigned_at` = wann zugewiesen.
- **Break** – Pause einer Person in einer Tour (`start`, `ended_at`).
- **Stop** – Besuch einer Adresse: `tour_id`, `location_id`, `zone_id` (leer = Ausreißer), `sequence`, `arrival` (geplant), `status`, `side`, `priority`, `special_type`, `warning` (Warnung aus der letzten Berechnung, z. B. „mit diesem Fahrzeug nicht erreichbar", leer = keine).
- **Stop_Crew** – Protokoll: wer wann am Stop war, bzw. Übergabe-Vermerk.
- **Problem** – Meldung zu einem Stop, `reported_by`, `resolved_at`.
- **Change_log** – Protokoll jeder Änderung: `account_id` (wer, leer = System), `table_name`, `row_id`, `field`, `old_value`, `new_value`, `changed_at`. Daraus entsteht der Teil „Korrekturen" im Tourbericht, und später der Sync.
- **Photo** – `uuid` (eindeutig über alle Fahrzeuge, für den späteren Sync nur fehlender Fotos). Foto immer zu einem Stop, optional zu einem Problem. `is_location_info` = gilt für die Adresse. Nur wenige Fotos, gespeichert als `bytea` direkt in der DB (geringe Last).
- **Delivery_note** – gescannter Lieferschein, aus dem der Stop entsteht (`source_pdf` = Dateipfad).

**Gruppe Routing**
- **Route** – Ergebnis einer Berechnung: `tour_id`, `variant` (`constrained` = mit Beschränkungen, `unconstrained` = ohne), `is_active` (welche der Fahrer fährt), `geom`, `length_m`, `duration_s`, `calculated_at`. Ersetzt den früheren Route-Cache an `Tour`. Aus beiden Varianten ergibt sich der Umweg durch die Beschränkungen.
- **Route_warning** – Warnungen zu Straßenabschnitten der Route: `route_id`, `osm_way_id` (logisch), `kind` (`underpass_no_height`, `missing_value`, `private_way`), `severity` (`info`, `warning`, `critical`), `message`.
- **Zone** – `tour_id`, `name`, `kind` (`boundary`, `cluster`, `split`, `corridor`), `sequence`, `area` (Polygon), `entry_point` (Punkt), `created_by`.
- **Street_override** – eigene Korrekturen pro Abschnitt, überleben jeden Import: Abschnitt (3 OSM-IDs, gemeinsam eindeutig), `serve_mode`, `max_height`, `max_width`, `max_weight`, `width`, `lanes`, `access`, `passes_under` (alle leer = keine Korrektur), `created_by`. Ersetzt das frühere `Street_rule`.
- **Street_attribute** – Werte aus dem Parser **pro Abschnitt** (eine Zeile pro `ways`-Zeile, Schlüssel = 3 OSM-IDs), inkl. `passes_under`. Abgeleitete Daten: bei jedem Import geleert und neu gefüllt, der Schlüssel wird nie geändert.
- **ways**, **ways_vertices_pgr**, **configuration** – Import von osm2pgrouting. `configuration` übersetzt `ways.tag_id` in die Straßenklasse (`tag_key`/`tag_value`, z. B. `highway`/`residential`), gebraucht für die Standardregel.

**Verbindungen (Krähenfuß):** Der Krähenfuß sitzt immer an der Tabelle mit dem Fremdschlüssel. An der referenzierten Tabelle steht `||` bei Pflicht-FK und `|o` bei nullable FK. Gestrichelt = logisch über OSM-ID.

### 10. Regeln für die Migrationen

- **Nullable:** `Tour_Crew.co_driver_id`, `Stop.zone_id`, `Stop.special_type`, `Stop.warning`, `Stop_Crew.originally_assigned_to`, `Stop_Crew.arrived_at`, `Stop_Crew.finished_at`, `Problem.resolved_at`, `Photo.problem_id`, `Account.crew_id`, `Change_log.account_id`, `Route_warning.osm_way_id`, `Location`-Snap-Felder, alle `Street_attribute`- und `Street_override`-Werte.
- **Pflicht (NOT NULL):** u. a. `Photo.stop_id`, `Stop.tour_id`, `Stop.location_id`, `Route.tour_id`.
- **UNIQUE:** `Crew_member.service_number`; `Photo.uuid`; `Account.username`; `Account.crew_id`; `Street_override (osm_way_id, from_node_osm_id, to_node_osm_id)`; pro Tour nur ein `start`- und ein `end`-Stop (partieller Index, z. B. `UNIQUE (tour_id) WHERE special_type = 'start'`); pro Tour höchstens eine aktive Route (`UNIQUE (tour_id) WHERE is_active`).
- **CHECK:**
  - `Stop_Crew`: `finished_at >= arrived_at`; `finished_at` nur, wenn `arrived_at` gesetzt ist; Zeile ohne Zeiten nur mit `originally_assigned_to` (Übergabe-Vermerk).
  - `Tour_Crew.co_driver_id <> crew_id`; `Stop_Crew.originally_assigned_to <> crew_id`.
  - Wertelisten für `Stop.status`, `Stop.side`, `Stop.special_type`, `Tour.cargo_type`, `Zone.kind`, `Street_override.serve_mode`, `Route.variant`, `Route_warning.kind`, `Account.role`, `created_by`.
- **Zeitpunkte** als `timestamptz` (Datum + Uhrzeit). Nur `Tour.date` und `Delivery_note.date` sind `date`. Touren über Mitternacht sind damit kein Problem.
- **Geometrie** mit festem SRID (z. B. `geometry(Point, 4326)`), GiST-Index auf allen Geometrie-Spalten.
- **Index** auf `Location (way_osm_id, from_node_osm_id, to_node_osm_id)` und `Street_attribute.osm_way_id`.
- Tabellennamen mit Unterstrich (`stop_crew`), keine Bindestriche. Nicht `user` als Tabellenname (reserviert in PostgreSQL), deshalb `account`.
- **Extensions:** `CREATE EXTENSION postgis;` und `CREATE EXTENSION pgrouting;`. `gen_random_uuid()` für `Photo.uuid` ist seit PostgreSQL 13 eingebaut.

**Automatische Tests (`tests/db/`, z. B. pytest gegen die Docker-DB, jeder Test in einer Transaktion, die zurückgerollt wird).** Jeder Test versucht, eine Regel zu brechen, und erwartet einen Fehler:
- zweite `Street_override` für denselben Abschnitt
- zweiter `Crew_member` mit gleicher `service_number`, zweiter `Account` mit gleichem `username`
- `Photo` ohne `stop_id`
- `Tour_Crew` mit `co_driver_id` = `crew_id`
- `Stop` ohne Tour oder mit nicht vorhandener Adresse
- zwei `start`-Stops in einer Tour
- ungültige Werte (z. B. `Stop.status = 'xyz'`)
- dazu Positivtests: gültige Daten lassen sich anlegen
- nach jedem OSM-Import: Bericht, welche `Street_override` keinen Abschnitt mehr findet

### 10a. Datentypen, Einheiten und Import-Namen (Kommentar im Issue „DB-routing-model")

Das ERD nutzt vereinfachte Typen, damit es lesbar bleibt. In den Migrationen gilt:

| Im ERD | In PostgreSQL | Wofür |
|---|---|---|
| `time` | `timestamptz` | alle Zeitpunkte (`created_at`, `arrived_at`, `assigned_at`, `calculated_at` …) |
| `date` | `date` | nur `Tour.date`, `Delivery_note.date` |
| `int` bei Höhe, Breite, Gewicht | `numeric` | `Vehicle`, `Street_attribute`, `Street_override` |
| `geometry` | `geometry(Point/LineString/Polygon, 4326)` | Punkte (`Location.geom`, `Zone.entry_point`), Linien (`Route.geom`), Flächen (`Zone.area`) |
| `uuid` | `uuid` mit Default `gen_random_uuid()` | `Photo.uuid` |
| `bytea` | `bytea` | `Photo.data` |
| `text`/`varchar` für Wertelisten | `text` + CHECK | `status`, `side`, `kind`, `role` … |

**Einheiten:** Höhen und Breiten in m; Gewichte in t (`Vehicle.weight` = zulässiges Gesamtgewicht); `Route.length` in m; `Route.duration` in s; `Location.edge_fraction` 0 bis 1; `maxspeed_*` in km/h.

**Import-Namen:** Im ERD lesbar, in der DB genau wie von osm2pgrouting angelegt: `Ways` → `ways`, `Ways_vertices_pgr` → `ways_vertices_pgr`, `Configuration` → `configuration`, `source_osm_id`/`target_osm_id` → `source_osm`/`target_osm`, `length` → `length_m`, für Fahrzeit `cost_s`/`reverse_cost_s` (`cost`/`reverse_cost` = Länge in Grad, nicht verwenden).

**Regeln, die man im ERD nicht sieht:**
- `Account.crew_id` UNIQUE (ein Account pro Fahrer).
- `Stop.zone_id` muss zu einer Zone derselben Tour gehören: `UNIQUE (id, tour_id)` auf `Zone`, `FOREIGN KEY (zone_id, tour_id) REFERENCES Zone (id, tour_id)` auf `Stop`.
- `Photo.problem_id` muss zu einem Problem desselben Stops gehören: `UNIQUE (id, stop_id)` auf `Problem`, `FOREIGN KEY (problem_id, stop_id) REFERENCES Problem (id, stop_id)` auf `Photo`.
- Ist `zone_id` bzw. `problem_id` NULL, prüft PostgreSQL die Regel nicht, Stops ohne Zone und Fotos ohne Problem bleiben möglich.

### 11. Kritische Prüfung (2026-11-27 abends)

**Im ERD geändert:**
1. Start, Ende und Depot fehlten → Spezial-Stops `start`, `depot`, `end`.
2. `Photo.location_id` hätte `Stop.location_id` verdoppelt → Foto gehört entweder zum Stop oder zur Adresse, `stop_id` nullable, CHECK.
3. Einheiten Fahrzeug (cm/kg) und Straße (m/t) passten nicht zusammen → beide m und t.
4. `ways.cost` ist die Länge in Grad → für Fahrzeit `cost_s` / `reverse_cost_s`.
5. `Street_override`: Abschnitt nur einmal (UNIQUE).
6. `Location.way_osm_id` nullable (Snapping kann fehlschlagen).
7. Login fehlte (Server-Diagramm hat „Auth + User") → neue Tabelle `Account`.
8. Eine Tour holt nur eine Art ab → `Tour.cargo_type`.
9. Wer hat was geändert → neue Tabelle `Change_log`.
10. Foto-Sync später → `Photo.uuid`.

**Zweite Prüfung (20:30):**
11. Foto hat immer einen Stop → `stop_id` Pflicht, `location_id` entfernt, dafür `is_location_info`.
12. `skipped` bekommt eine `Stop_Crew`-Zeile mit gleicher Ankunfts- und Abfahrtszeit.
13. Route-Ergebnis und Vergleichsroute brauchten einen Platz → `Route` (zwei Varianten) statt `Tour.route_geom`.
14. Warnungen brauchten einen Platz → `Route_warning` und `Stop.warning`.
15. Standardregel braucht die Straßenklasse → `configuration` von osm2pgrouting und `ways.tag_id` ins ERD.
16. Aus welcher Adresse Start, Depot und Ende entstehen → `Location.is_depot`.
17. Vollständigkeit geprüft: Alles für Berechnung, Ergebnis und Bearbeiten der Stops liegt in PostgreSQL. Nicht gespeichert werden nur Matrix und GPS-Position.

**Im Team entschieden:**
- Login kommt dazu (`Account`). Erst nach der Anmeldung ist die eigene Tour sichtbar. Jede Änderung wird mit dem Account im `Change_log` protokolliert.
- Eine Tour bedient genau einen Behältertyp, `counter` zählt allgemein.
- Ladekapazität vorerst nicht → Nice-to-have-Issue. Später eine nullable Spalte `Vehicle.capacity` (eine Migration, kein Umbau).
- Korrekturen der Fahrer bleiben in der Datenbank des jeweiligen Fahrzeugs. Für Version 1 reicht das, weil alles eigenständig läuft. Der Tourbericht listet, was geändert wurde. Sync kommt später.
- Wichtig dafür: Die Fahrzeug-DB wird **nicht** automatisch aktualisiert. Nur bei wichtigen Neuerungen fragt die App, ob sie eingespielt werden sollen; bestehende Korrekturen bleiben.
- Fotos nur minimal, bleiben als `bytea` in der DB, mit `uuid`. Später beim Sync nur fehlende Fotos übertragen.
- Status `skipped` = vom Fahrer erledigt markiert, ohne Halt.
- Grenzwerte als Startwerte festgelegt (Abschnitt 7a).
- Für alle Regeln der DB gibt es automatische Tests.

**Prüfung des draw.io-ERD (23:00):**
- Alle Tabellen, Spalten und Krähenfüße stimmen mit dem Modell überein.
- Korrigiert: Photo → Problem `|o`, Route_warning → ways `|o`, ways_vertices_pgr → ways durchgezogen (echter FK), Tippfehler `created_by`.
- Vor dem Push: `Break.tour_id` von `text` auf `int`; Beschriftung „creadet by" → „created by"; `Photo.UUID` → `uuid`.
- Datentypen, Einheiten und Import-Namen stehen als Kommentar im Issue (Abschnitt 10a), nicht im ERD.

**Noch offen:** nichts Grundsätzliches. Die Grenzwerte werden im Kerneltest geprüft und bei Bedarf angepasst.

### 12. Bekannte Grenzen (für die Doku)

- Übergabe kommt offline nicht an → Fahrer hakt selbst ab.
- Korrekturen der Fahrer bleiben im jeweiligen Fahrzeug. Andere Fahrzeuge sehen sie erst mit dem späteren Sync. Der Tourbericht listet sie.
- Fehlende OSM-Werte → Straße gilt als befahrbar, nur Warnung.
- Nach einem OSM-Import können einzelne Straßenregeln nicht mehr passen.
- OR-Tools ist nahezu optimal, nicht beweisbar optimal. Zonen kosten an den Grenzen Qualität.
- Die Datenbank speichert nur das Ergebnis (Stops, wer was gemacht hat), nicht die Planung des Disponenten.

### 13. Ausblick (Erweiterung / Nice to have)

- Zentrale Version: Disponenten-Client, Master-DB, Historie, UUIDs.
- Zonengrenzen aus Gemeindegrenzen, Flüssen, Bahnlinien (`Zone_boundary`).
- Sync: Korrekturen der Fahrer zwischen Fahrzeugen und Master-DB abgleichen.
- Foto-Sync: nur fehlende Fotos übertragen, erkannt an `Photo.uuid`.
- Ladekapazität (`Vehicle.capacity`), damit OR-Tools Depot-Fahrten selbst plant.
- Live-Traffic in der Optimierung.

### 14. Nächste Schritte

- GitHub-Issue (Nice to have): Ladekapazität `Vehicle.capacity`.
- ERD in draw.io nachziehen (Vorlage: Artifact „VobiTour ERD").
- Versionierte SQL-Migrationen (`db/migrations/`) inkl. Regeln aus Abschnitt 10.
- Automatische DB-Tests (`tests/db/`) für jede Regel, zusätzlich: zwei Fotos mit gleicher `uuid`, `finished_at` vor `arrived_at`, zwei aktive Routen in einer Tour, zwei `start`-Stops.
- Parser für `Street_attribute`, Import-Skript, Seed-Generator (`db/seed/`).
- View `routing_edges` mit Beschränkungen, Strafaufschlag und Kosten in Sekunden.
- Kerneltest: 20, 50, 100 Stops pro Zone und 400 am Stück; mit und ohne Beschränkungen; auf dem kleinen PC im Fahrzeug.
- Performance-Report schreiben.
- Client-DB-Schema (SQLite, Outbox) dokumentieren.

### 14a. Weg zum ersten Testlauf (neues Issue „DB-routing")

1. Docker-Container mit PostgreSQL + PostGIS + pgRouting starten, `CREATE EXTENSION postgis; CREATE EXTENSION pgrouting;`.
2. OSM-Auszug: Österreich von Geofabrik, mit `osmium` auf Gleisdorf + Umgebung zuschneiden.
3. Import mit osm2pgrouting (`ways`, `ways_vertices_pgr`, `configuration`), prüfen: `cost_s`, `maxspeed_*`, Zufahrten (`service`) mit drin.
4. Parser füllt `Street_attribute` (inkl. Brücken/Tunnel → `passes_under`).
5. Migrationen der eigenen Tabellen einspielen.
6. Seed: Testfahrer und Accounts (z. B. mit Faker `de_AT`), Testfahrzeuge (darunter eines über 3,5 m Höhe), Depot, Testadressen aus echten OSM-Hausnummern in Gleisdorf, Testtouren mit 20 / 100 / 400 Stops.
7. Stop-Snapping auf die Abschnitte (Seite, Position).
8. Matrix + OR-Tools rechnen, beide Varianten in `Route` speichern, Warnungen prüfen.
9. DB-Tests und Messung der Rechenzeiten.

### 15. Relevante Dateien und Pfade

- `projekt-stand.md` – dieser Snapshot
- `erd.png` / draw.io-Datei – finales ERD
- `issue-db-routing-model.md` – Text des neuen Issues „DB-routing-model" (inkl. aller Entscheidungen, Grenzwerte, Quellen)
- Issue #26 „DB-evaluate" – abgeschlossen mit Kommentar (Stammdaten fertig)
- Artifact „VobiTour ERD" – gezeichnetes ERD mit Krähenfüßen (Betrieb / Routing)
- Architekturdiagramme: Server, Client, Client-Server
- GitHub-Issues #21–#26 (Milestone V 4.0)
- Geplante Struktur: `db/migrations/`, `db/seed/`, `tests/db/`

### 16. Änderungen seit letztem Stand (17:30)

- Unterführungen: `Street_attribute.passes_under`, `Route_warning.severity`, Warnung `underpass_no_height`; Abschnitt „Die fertige Tour" (4.8).
- ERD final in draw.io (Farben, Legende, Gruppen Business/Routing), geprüft und bereit für #21; Kommentar mit Datentypen, Einheiten, Import-Namen und zusammengesetzten Fremdschlüsseln.
- `Street_attribute` pro Abschnitt (abgeleitet, bei jedem Import neu); `Street_rule` → `Street_override` mit allen Straßenwerten zum Nachtragen; Ablauf für Unterführungen.
- Issue #26 (Stammdaten) abgeschlossen, neues Issue „DB-routing" für Routing und Stop-Verwaltung; Weg zum ersten Testlauf (14a); `pgr_withPointsCostMatrix` für Stops mitten auf Abschnitten.
- Zweite Prüfung: Foto immer mit Stop (`is_location_info`), `skipped` mit gleichen Zeiten, neue Tabellen `Route`, `Route_warning`, `configuration`, neue Spalten `Stop.warning`, `Location.is_depot`.
- Status `skipped`, `Change_log`, `Photo.uuid`, Grenzwerte (7a), keine automatische Stammdaten-Aktualisierung.
- Team-Entscheidungen: Login (`Account`), `Tour.cargo_type`, keine Kapazität vorerst, Korrekturen bleiben im Fahrzeug, Fotos in der DB, automatische Tests.
- Spezial-Stops `start`, `depot`, `end`.
- Fahrzeug-DB wird nicht täglich überschrieben, Stammdaten per Seed-Skript.
- Zonen: Arten `boundary`, `cluster`, `split`, `corridor`; Grenzstraßen = `direction_only`; Ausreißer-, Mini-Zonen- und Abschnitt-Regel; Zonen nur wenn sinnvoll.
- Straßen: Standardregel (60 km/h, Breite), `Street_override` nur Abweichungen, `Street_attribute` mit `width`, `lanes`, `access`.
- Berechnung: Kosten in Sekunden, Knoten pro Abschnitt + Seite, Erreichbarkeit über fehlende Matrix-Paare, zweite Route ohne Beschränkungen.
- Private Wege: alles importieren, Strafaufschlag statt Sperre.
- Adressen: Pin an Einfahrt bzw. öffentliche Straße, Adresse bleibt gleich; Adress-Fotos.
- Kritische Prüfung mit 6 Änderungen und 7 offenen Entscheidungen.
- ERD als gezeichnete Seite mit Gruppen Betrieb und Routing.

---

## Ältere Stände

## Aktueller Stand (2026-11-27 17:30)

### Architektur / Übersicht

Diplomarbeit-App **VobiTour**: Constraint-basierte dynamische Tourenoptimierung für Liefer- und Entsorgungsfahrten (auch generalisierbar für Post, Müllabfuhr, Ambulanz).

**Team:** Mario Vogrin (Backend), Gabriel Binderbauer (Frontend Qt/QML)

**Version 1 (Ziel der Diplomarbeit):**
- Jeder Fahrer arbeitet eigenständig. Server + PostgreSQL laufen im Fahrzeug (kleiner PC).
- Vor Tourbeginn wird die aktuelle Datenbank ins Fahrzeug kopiert. Danach fährt jedes System selbständig.
- Am Ende jeder Tour wird ein Bericht ausgegeben (= Historie).
- Übergaben von Stops markieren die Fahrer selbst.

**Erweiterung (nur auf Wunsch einer Firma):**
- Disponenten-Client (Web-App oder CLI) + Master-DB, die vor Tourbeginn aktualisiert und in die Fahrzeuge kopiert wird.
- Historie aus den Tourberichten.
- Das Datenmodell bleibt gleich, nur `created_by = dispatcher` kommt dazu.

**Rollen im Client/Server-Modell:**
- Fahrer-Client (Qt/QML): Tour, Stopps (verschieben, hinzufügen, Depot-Rückkehr, fertig/`reassigned`), Scan, Profil, Problem melden/lösen, Zonen bearbeiten.
- Disponent-Client (optional): Zonen und Stops anlegen, Stops von Fahrer A zu Fahrer B übergeben.
- Server: REST-API, PostgreSQL + PostGIS + pgRouting + OR-Tools.
- Kommunikation: HTTP/JSON, Server-IP im Client umstellbar.

**Aktueller Fokus:** Finales ERD inkl. Zonen und Straßenregeln → Migrationen → Kernel-Test.

### Konfigurationen und Setups

- UI: Qt/QML
- Server-DB: PostgreSQL + PostGIS + pgRouting (Docker)
- Client-DB: SQLite
- Optimierer: OR-Tools
- Straßennetz: OSM via osm2pgrouting (legt `ways` und `ways_vertices_pgr` selbst an, nicht in den eigenen Migrationen)
- Constraints (maxheight/maxwidth/maxweight): eigener Parser → Tabelle `Street_constraint`
- Testdaten: realistischer OSM-Extrakt + Seed für Stops
- Migrations: versionierte SQL-Dateien

### Wichtige Entscheidungen

- **ERD-Änderungen (2026-11-27 Nachmittag):**
  - Neu: Tabelle `Zone` (`tour_id`, `name`, `sequence`, `area`, `entry_point`, `created_by`)
  - Neu: `Stop.zone_id` (nullable), `Stop.side` (left/right/any)
  - Neu: `Problem.resolved_at` (leer = offen)
  - Neu: `Location`-Snap-Felder (`way_osm_id`, `from_node_osm_id`, `to_node_osm_id`, `edge_fraction`)
  - Neu: `Tour.route_geom` + `route_calculated_at` (Route-Cache)
  - Neu: `Street_rule` (`serve_mode` both/direction_only pro Abschnitt) und `Street_constraint`
  - Entfernt: `Stop.is_special` (aus `special_type` ableitbar), `Stop_Crew.completed`, `Tour_Crew.role`, `Tour_Crew.assigned_at`
  - Umbenannt: `Location.description` → `note` (Hinweis des Fahrers), `Break.end` → `ended_at`, `Cargo.typ` → `cargo_type`
  - `Crew_member.service_number` UNIQUE (gleiche Fahrer in jeder Fahrzeug-DB)
  - `Photo.problem_id` nullable (Foto auch ohne Problem)
  - Ein optionaler Beifahrer pro Tour (`co_driver_id`), kein Mehrfach-Modell
  - Alle Zeitpunkte als Zeitstempel mit Datum, nur `Tour.date` bleibt `date`
  - OSM-Tabellen werden über OSM-IDs verknüpft (logisch, kein FK), weil sich `gid`s bei jedem Neuimport ändern
- **Übergabe von Stops:**
  - Beim ursprünglichen Fahrer: Stop auf `reassigned`, `Stop_Crew`-Zeile mit `crew_id` = neuer Fahrer, `originally_assigned_to` = ursprünglicher Fahrer, ohne Zeiten (= Übergabe-Vermerk, keine Fahrt).
  - Beim neuen Fahrer: Stops ganz normal neu angelegt, `originally_assigned_to` leer.
  - Sonst entsteht eine `Stop_Crew`-Zeile nur beim tatsächlichen Halt.
- **Zonen:**
  - Vorschlag vom System: Teilung entlang natürlicher Grenzen aus OSM (Hauptstraßen, Bahn, Flüsse), danach automatisches Teilen zu großer Zonen.
  - Grenzen: maximale Stop-Anzahl und maximale Ausdehnung pro Zone.
  - Fahrer (später auch Disponent) kann Zonen ändern, neu zeichnen und Stops darin anlegen.
  - `entry_point` pro Zone, z. B. wegen Einbahnstraßen.
  - **Ausreißer-Regel:** Liegt ein Stop zu weit von allen anderen entfernt, bekommt er keine Zone (`zone_id = NULL`) und `side = any`. Er wird als einzelner Punkt zwischen den Zonen eingeplant. Umsetzbar mit `ST_ClusterDBSCAN`, das Punkte ohne genug Nachbarn keinem Cluster zuordnet.
- **Straßen (Arc Routing / CARP):**
  - Hauptfall sind Straßen (Müllabfuhr). Wo es nur um Punkte geht (Ambulanz), bleibt es einfaches Punkt-Routing.
  - Pro Abschnitt (Kreuzung zu Kreuzung): `both` oder `direction_only`.
  - Stop-Snapping am Server (Pipeline nach Geocoding) legt jeden Stop auf eine Kante und berechnet `side`. Fahrer kann auf `any` stellen.
  - Begriffe trennen: „Stop-Snapping" (Server) vs. „GPS-Snapping" (Client).
- **Berechnung:**
  - Dijkstra (`pgr_dijkstraCostMatrix`) füllt pro Zone eine Matrix mit gerichteten Kanten (exakt).
  - OR-Tools rechnet zuerst die Reihenfolge der Zonen, dann jede Zone mit festem Start (`entry_point`) und festem Ende (Richtung nächste Zone).
  - Ändert sich ein Stop, wird nur diese Zone neu gerechnet.
  - OR-Tools liefert „nahezu optimal" im Zeitlimit. Bis ca. 15–20 Stops pro Zone wäre exakt möglich (Held-Karp).
- **GitHub-Issues (Milestone V 4.0):** #21–#26 (inkl. DB-evaluate)
- Live-Traffic nur für Anzeige/ETA, nicht für Optimierung

### Offene Punkte und nächste Schritte

- Konkrete Zahlen festlegen: max. Stops und Ausdehnung pro Zone, Abstand für die Ausreißer-Regel, Akzeptanzzeit der Berechnung
- Regel für fehlende OSM-Constraint-Werte (Warnung vs. Block) festlegen
- Prüfen, wie maxheight/maxwidth/maxweight aus OSM kommen (osm2pgrouting-Tags oder eigener Parser)
- ERD in draw.io nachziehen (Zone, Street_rule, Street_constraint, OSM-Block)
- Versionierte SQL-Migrationen aus dem finalen ERD erstellen
- Seed-Generator und Import-Skript für Straßennetz + Stops umsetzen
- Kernel-Test: 20, 50, 100 Stops pro Zone und 400 Stops am Stück
- Performance-Report schreiben
- Doku: Absatz „Ausblick: zentrale Version"; bekannte Grenzen (Übergabe kommt offline nicht an; Pin-Korrekturen gehen beim Kopieren der DB verloren)

### Relevante Dateien und Pfade

- `projekt-stand.md` – dieser Snapshot
- Artifact „VobiTour ERD" – vollständiges ERD mit Krähenfüßen (Mermaid, in draw.io importierbar)
- Architekturdiagramme: Server, Client, Client-Server
- GitHub-Issues #21–#26 (Milestone V 4.0)
- Geplante Struktur: `db/migrations/`, `db/seed/`, `tests/db/`

### Änderungen seit letztem Stand

- Scope festgelegt: Version 1 = Fahrer eigenständig, Erweiterung = Disponent + Master-DB
- Ablauf der Stop-Übergabe festgelegt
- Zonenkonzept inkl. Ausreißer-Regel festgelegt
- Straßenregeln (Arc Routing) und Stop-Snapping festgelegt
- Berechnungsablauf (Matrix pro Zone, Zonenreihenfolge, fester Start/Ende) festgelegt
- ERD um Zone, Street_rule, Street_constraint, Snap-Felder, Route-Cache, `resolved_at` erweitert; Redundanzen entfernt


## Aktueller Stand (2026-11-27 13:35)

### Architektur / Übersicht

Diplomarbeit-App **VobiTour**: Constraint-basierte dynamische Tourenoptimierung für Liefer- und Entsorgungsfahrten (auch generalisierbar für Post, Müllabfuhr, Ambulanz).

**Team:** Mario Vogrin (Backend), Gabriel Binderbauer (Frontend Qt/QML)

**Client/Server-Modell (bestätigt):**
- Client (Qt/QML): UI, lokale Navigation (GPS-Snapping, Abweichung), SQLite-Cache + Outbox, Sync.
- Server: REST-API, PostgreSQL + PostGIS + pgRouting + OR-Tools für Optimize/Recalculate.
- Kommunikation: HTTP/JSON, Server-IP im Client umstellbar (lokal oder remote).

**Aktueller Fokus:** Finalisiertes und reviewtes ERD + Vorbereitung Kernel-Test (OSM-Import → Constraint-Filter → Matrix → OR-Tools → Geometrie).

### Konfigurationen und Setups

- UI: Qt/QML
- Server-DB: PostgreSQL + PostGIS + pgRouting (Docker)
- Client-DB: SQLite
- Optimierer: OR-Tools
- Straßennetz: OSM via osm2pgrouting + Constraint-Filter (maxheight/maxwidth/maxweight)
- Testdaten: realistischer OSM-Extrakt + Seed für Stops
- Migrations: versionierte SQL-Dateien

### Wichtige Entscheidungen

- **Finales generalisiertes Datenmodell (2026-11-25 / Review 2026-11-27):**
  - Tabellen umbenannt: `Driver` → `Crew_member`, `Container` → `Cargo`, `Tour-Driver` → `Tour-Crew`, `Stop-Driver` → `Stop-Crew`
  - Beifahrer pro Tour: `Tour-Crew.co_driver_id` (nullable, FK auf `Crew_member`)
  - Ursprüngliche Zuweisung vs. tatsächliche Ausführung: `Stop-Crew.originally_assigned_to` (FK auf `Crew_member`)
  - Allgemeiner Zähler an Tour: `Tour.counter`
  - Spezial-Stops mit Priorität: `Stop.is_special`, `Stop.priority`, `Stop.special_type`
  - Cargo gehört zu Client + Location
  - Photo als eigene Tabelle mit BLOB
  - m:n über Zwischentabellen für Crew↔Tour und Vehicle↔Tour
  - Einspringen einzelner Stopps über `Stop-Crew`
- **ERD-Review (Gabriel, 2026-11-27):**
  - Fehlende Beziehungslinien für `co_driver_id` und `originally_assigned_to` ergänzt
  - Krähenfüße an den Zwischentabellen korrigiert
  - Datenbankstruktur war bereits korrekt, nur die Zeichnung wurde angepasst
- **Nächster Meilenstein:** DB-Foundation + Routing-Performance (100 Stops)
  - Versionierte Migrations des finalen ERDs
  - Straßennetz-Import + Testdaten-Generator
  - Messung der Pipeline: Edge-Filter → `pgr_dijkstraCostMatrix` → OR-Tools → Geometrie
- **GitHub-Issues (Milestone V 4.0):** #21–#26 (inkl. DB-evaluate)
- Live-Traffic nur für Anzeige/ETA, nicht für Optimierung

### Offene Punkte und nächste Schritte

- Akzeptanzzeit für 100-Stops-Route konkret festlegen
- Versionierte SQL-Migrationen aus dem finalen ERD erstellen
- Regel für fehlende OSM-Constraint-Werte (Warnung vs. Block) festlegen
- Seed-Generator und Import-Skript für Straßennetz + Stops umsetzen
- Kernel-Test in der CLI vorbereiten und durchführen
- Performance-Report schreiben

### Relevante Dateien und Pfade

- `/workspace/artifacts/projekt-stand.md` – dieser Snapshot
- GitHub-Issues #21–#26 (Milestone V 4.0)
- Finale ERD-Diagramme (inkl. korrigierter Krähenfüße und Beziehungslinien)
- Geplante Struktur: `db/migrations/`, `db/seed/`, `tests/db/`

### Änderungen seit letztem Stand

- ERD-Review von Gabriel eingearbeitet
- Fehlende Beziehungslinien für `co_driver_id` und `originally_assigned_to` ergänzt
- Krähenfuß-Notation an den Zwischentabellen (Tour-Crew, Stop-Crew, Tour-Vehicle) korrigiert
- Bestätigung, dass die Datenbankstruktur korrekt ist

## Aktueller Stand (2026-11-25 21:04)

### Architektur / Übersicht

Diplomarbeit-App **VobiTour**: Constraint-basierte dynamische Tourenoptimierung für Liefer- und Entsorgungsfahrten (auch generalisierbar für Post, Müllabfuhr, Ambulanz).

**Team:** Mario Vogrin (Backend), Gabriel Binderbauer (Frontend Qt/QML)

**Client/Server-Modell (bestätigt):**
- Client (Qt/QML): UI, lokale Navigation (GPS-Snapping, Abweichung), SQLite-Cache + Outbox, Sync.
- Server: REST-API, PostgreSQL + PostGIS + pgRouting + OR-Tools für Optimize/Recalculate.
- Kommunikation: HTTP/JSON, Server-IP im Client umstellbar (lokal oder remote).

**Aktueller Fokus:** Finalisiertes generalisiertes Datenmodell + Vorbereitung Kernel-Test (OSM-Import → Constraint-Filter → Matrix → OR-Tools → Geometrie).

### Konfigurationen und Setups

- UI: Qt/QML
- Server-DB: PostgreSQL + PostGIS + pgRouting (Docker)
- Client-DB: SQLite
- Optimierer: OR-Tools
- Straßennetz: OSM via osm2pgrouting + Constraint-Filter (maxheight/maxwidth/maxweight)
- Testdaten: realistischer OSM-Extrakt + Seed für Stops
- Migrations: versionierte SQL-Dateien

### Wichtige Entscheidungen

- **Finales generalisiertes Datenmodell (2026-11-25, nach Durchsprache Ambulanz/Post/Müll):**
  - Tabellen umbenannt für Generalisierung: `Driver` → `Crew_member`, `Container` → `Cargo`, `Tour-Driver` → `Tour-Crew`, `Stop-Driver` → `Stop-Crew`
  - Beifahrer pro Tour: `Tour-Crew.co_driver_id` (nullable, FK auf `Crew_member`)
  - Ursprüngliche Zuweisung vs. tatsächliche Ausführung: `Stop-Crew.originally_assigned_to`
  - Allgemeiner Zähler an Tour: `Tour.counter` (z. B. gelbe Säcke)
  - Spezial-Stops mit Priorität: `Stop.is_special`, `Stop.priority`, `Stop.special_type` (für Depot-Rückkehr / Ausleeren)
  - Cargo gehört zu Client + Location
  - Photo als eigene Tabelle mit BLOB
  - m:n über Zwischentabellen für Crew↔Tour und Vehicle↔Tour
  - Einspringen einzelner Stopps / Straßen über `Stop-Crew`
  - Probleme mit oder ohne Foto möglich
- **Nächster Meilenstein:** DB-Foundation + Routing-Performance (100 Stops)
  - Versionierte Migrations des finalen ERDs
  - Straßennetz-Import + Testdaten-Generator
  - Messung der Pipeline: Edge-Filter → `pgr_dijkstraCostMatrix` → OR-Tools → Geometrie
- **GitHub-Issues (Milestone V 4.0):** #21–#26 (inkl. DB-evaluate)
- Live-Traffic nur für Anzeige/ETA, nicht für Optimierung

### Offene Punkte und nächste Schritte

- Akzeptanzzeit für 100-Stops-Route konkret festlegen
- Versionierte SQL-Migrationen aus dem finalen ERD erstellen
- Regel für fehlende OSM-Constraint-Werte (Warnung vs. Block) festlegen
- Seed-Generator und Import-Skript für Straßennetz + Stops umsetzen
- Kernel-Test in der CLI vorbereiten und durchführen
- Performance-Report schreiben

### Relevante Dateien und Pfade

- `/workspace/artifacts/projekt-stand.md` – dieser Snapshot
- GitHub-Issues #21–#26 (Milestone V 4.0)
- Finale ERD-Diagramme (mehrere Iterationen bis erd-7.png)
- Geplante Struktur: `db/migrations/`, `db/seed/`, `tests/db/`

### Änderungen seit letztem Stand

- ERD mehrfach überarbeitet und finalisiert (Crew_member, Cargo, Tour-Crew, Stop-Crew)
- Beifahrer pro Tour über `co_driver_id` in Tour-Crew
- `originally_assigned_to` in Stop-Crew ergänzt
- `Tour.counter` für allgemeinen Zähler ergänzt
- Spezial-Stops mit Priorität (`is_special`, `priority`, `special_type`) für Depot-Rückkehr
- Durchsprache der Abläufe für Ambulanz, Post und Müllabfuhr abgeschlossen – Modell deckt die Fälle ab
- Issue #26 (DB-evaluate) mit den Änderungen kommentiert

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
