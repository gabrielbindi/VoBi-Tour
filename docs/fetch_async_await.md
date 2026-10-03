Neues Konzept: fetch() und async/await

Bisher hast du nur mit Daten gearbeitet, die schon im Code standen. Jetzt holen wir Daten aus dem Internet — das braucht zwei neue Werkzeuge.

fetch(url) schickt eine Anfrage an eine Webadresse und gibt die Antwort zurück. Das Problem: Eine Netzwerk-Anfrage dauert eine gewisse Zeit (vielleicht 200 Millisekunden, vielleicht 2 Sekunden). JavaScript kann nicht einfach "warten", ohne dass die ganze Seite währenddessen einfriert.

async und await lösen das. Eine Funktion, die mit async function beginnt, darf das Wort await benutzen. await heißt: "Warte hier, bis die Antwort da ist, aber blockiere dabei nicht den Rest der App." Das ist wie eine Pause nur innerhalb dieser einen Funktion.

Was hier Schritt für Schritt passiert

async function ladeHaltestellenVonOverpass(): Das async davor ist nötig, sonst darfst du await innerhalb der Funktion gar nicht benutzen.

const abfrage = '...': Genau der Text, den du gerade in Overpass Turbo getestet hast, als JavaScript-String gespeichert.

await fetch(url, {...}): Schickt die Anfrage los und wartet, bis die Antwort da ist, bevor der Code weitergeht. method: 'POST' heißt, wir schicken Daten mit (die Abfrage), nicht nur eine einfache Adresse wie beim Kartenladen.

'data=' + encodeURIComponent(abfrage): Overpass erwartet die Abfrage in diesem speziellen Format. encodeURIComponent wandelt Sonderzeichen (wie ", [, ]) in eine Form um, die sicher über das Netzwerk geschickt werden kann.

await antwort.json(): Die rohe Antwort muss noch von Text in ein echtes JavaScript-Objekt umgewandelt werden. Auch das dauert kurz, deshalb wieder await.

daten.elements: Genau das Array, das du im Screenshot unter "elements": [ ... ] gesehen hast — eine Liste aller gefundenen Punkte.

if (punkt.tags && punkt.tags.name): Nicht jeder Punkt hat zwingend einen Namen (manche Haltestellen in OSM sind nur geometrisch erfasst, ohne name-Tag). Diese Prüfung filtert solche unbenannten Punkte raus, damit deine Liste keine leeren Namen hat.

haltestellen.push({...}): Baut aus den OSM-Daten genau die gleiche Objektform, die du schon kennst (name, lat, lng, verspaetung) — deshalb funktioniert der ganze restliche Code (Marker, Farben, Polling) unverändert weiter!

setzeAlleHaltestellen() am Ende: zeichnet die Marker, sobald die echten Daten da sind.

Wichtige Änderung: haltestellen darf sich jetzt ändern

Bisher war haltestellen eine feste const-Liste. Jetzt ersetzen wir sie komplett mit echten Daten, deshalb muss sie zu let werden:
