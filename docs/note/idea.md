## Architektur & Infrastruktur

- **Server-Standort konfigurierbar**  
  Client kann per IP auf Mini-PC im Fahrzeug oder Firmen-Server zeigen. Nicht beides gleichzeitig als Standard.

- **Offline-Verhalten**  
  Offline: Tour anzeigen, Status ändern, Stopps manuell einfügen (Fahrer wählt Position). Keine volle Neuberechnung. Sync bei Verbindung.

## Zonen & Optimierung

- **Zonen-Vorschlag**  
  System schlägt Zonen vor (PostGIS-Clustering). Disponent kann löschen, verschieben, neu zeichnen oder anpassen.

- **Zonen-Regeln für lange Straßen & Sackgassen**  
  Lange Straßen/Sackgassen als eigene Zonen oder klare Abschnitte (Anfang/Ende). Zonen am Land klein halten.

- **Straßenseiten-Logik**  
  Bevorzugt eine Seite komplett, dann die andere. Seitenwechsel nur bei sicheren Bedingungen (≤ 50 km/h, nicht zu breit, nicht wenn man sowieso zurückfährt).

- **Kleine Abzweigungen / Dörfer**  
  Beide Varianten rechnen (jetzt mitnehmen vs. später). Differenz anzeigen + Empfehlung („besser jetzt mitnehmen“).

- **Abweichungsanzeige**  
  Bei manueller Änderung sofort grober Richtwert. Echte Neuberechnung auf Knopfdruck. Differenz zur besten Route immer anzeigen.

## Constraints & Datenqualität

- **OSM-Constraint-Import + Parser**  
  Höhe/Breite/Gewicht aus OSM importieren und validieren. Originalwerte immer als Hinweis anzeigen.

- **Manuelle Korrektur durch Fahrer**  
  Fahrer kann Höhe/Breite/Gewicht während der Tour eintragen oder ändern (besonders bei fehlenden/falschen OSM-Daten). Änderung wird protokolliert.

- **Warnungen statt hartem Blockieren**  
  Bei fehlenden Werten oder großem Umweg Warnung anzeigen, nicht automatisch blockieren.

## Rollen & Reporting

- **Disponent als Supporter**  
  Kann Zonen anpassen und Daten korrigieren. Mehrere Disponenten sehen nur zugeordnete Fahrer.

- **Tour-Bericht**  
  Alle Überschreibungen (Seitenwechsel, Zonen-Änderungen, Constraint-Korrekturen) kurz und klar protokollieren. Report-Tool zur Anzeige.

## Nice-to-have

- **Lernendes Verhalten**  
  System merkt sich Fahrer-/Disponenten-Entscheidungen für ähnliche Straßen (z. B. Seitenwechsel abgelehnt).
