# Ionic + Vue – Komponenten-Übersicht

Sammlung aller wichtigen Ionic-UI-Komponenten nach Kategorie, mit kurzen Code-Beispielen zum Ausprobieren.

---

## 1. Eingabe-Elemente

### ion-input – Textfeld
```vue
<ion-input label="Name" label-placement="floating" placeholder="Dein Name"></ion-input>
```

### ion-textarea – mehrzeiliges Textfeld
```vue
<ion-textarea label="Nachricht" :rows="4" placeholder="Deine Nachricht"></ion-textarea>
```

### ion-select – Dropdown / Auswahlliste
```vue
<ion-select label="Verkehrsmittel" placeholder="Wähle aus">
  <ion-select-option value="bus">Bus</ion-select-option>
  <ion-select-option value="bahn">Bahn</ion-select-option>
  <ion-select-option value="tram">Tram</ion-select-option>
</ion-select>
```

### ion-checkbox – Checkbox
```vue
<ion-checkbox v-model="checked">Ich stimme zu</ion-checkbox>
```

### ion-toggle – An/Aus-Schalter
```vue
<ion-toggle v-model="darkMode">Dark Mode</ion-toggle>
```

### ion-radio / ion-radio-group – Radiobuttons
```vue
<ion-radio-group v-model="auswahl">
  <ion-radio value="option1">Option 1</ion-radio>
  <ion-radio value="option2">Option 2</ion-radio>
</ion-radio-group>
```

### ion-range – Schieberegler
```vue
<ion-range :min="0" :max="100" v-model="value">
  <ion-icon slot="start" :icon="volumeLowOutline"></ion-icon>
  <ion-icon slot="end" :icon="volumeHighOutline"></ion-icon>
</ion-range>

<!-- mit zwei Griffen, z.B. für eine Preis- oder Radius-Spanne -->
<ion-range dual-knobs="true" :min="0" :max="100"></ion-range>

<!-- mit Sprungmarken und sichtbaren Ticks -->
<ion-range :min="0" :max="100" :step="10" snaps="true" ticks="true"></ion-range>
```

### ion-datetime – Datum/Zeit-Picker
```vue
<ion-datetime presentation="date"></ion-datetime>
<ion-datetime presentation="time"></ion-datetime>
```

### ion-searchbar – Suchfeld
```vue
<ion-searchbar placeholder="Suchen..." v-model="suchbegriff"></ion-searchbar>
```

---

## 2. Listen & Karten

### ion-list + ion-item – Standard-Liste
```vue
<ion-list>
  <ion-item v-for="haltestelle in haltestellen" :key="haltestelle.id">
    <ion-label>{{ haltestelle.name }}</ion-label>
  </ion-item>
</ion-list>
```

### ion-item mit Icon, Detail-Pfeil, Klick
```vue
<ion-item button @click="oeffneDetails(haltestelle)">
  <ion-icon slot="start" :icon="busOutline"></ion-icon>
  <ion-label>{{ haltestelle.name }}</ion-label>
</ion-item>
```

### ion-card – Karten-Layout
```vue
<ion-card>
  <ion-card-header>
    <ion-card-title>Titel</ion-card-title>
    <ion-card-subtitle>Untertitel</ion-card-subtitle>
  </ion-card-header>
  <ion-card-content>
    Inhalt der Karte.
  </ion-card-content>
</ion-card>
```

### ion-avatar / ion-thumbnail – Bilder in Listen
```vue
<ion-item>
  <ion-avatar slot="start">
    <img src="avatar.jpg" />
  </ion-avatar>
  <ion-label>Name</ion-label>
</ion-item>
```

### ion-badge – kleine Status-Markierung
```vue
<ion-item>
  <ion-label>Nachrichten</ion-label>
  <ion-badge slot="end">3</ion-badge>
</ion-item>
```

### ion-chip – kleines Tag/Label-Element
```vue
<ion-chip>
  <ion-icon :icon="pricetagOutline"></ion-icon>
  <ion-label>Verspätung</ion-label>
</ion-chip>
```

---

## 3. Navigation & Struktur

### ion-tabs – Tab-Leiste unten
```vue
<ion-tabs>
  <ion-router-outlet></ion-router-outlet>
  <ion-tab-bar slot="bottom">
    <ion-tab-button tab="home" href="/home">
      <ion-icon :icon="homeOutline"></ion-icon>
      <ion-label>Home</ion-label>
    </ion-tab-button>
    <ion-tab-button tab="map" href="/map">
      <ion-icon :icon="mapOutline"></ion-icon>
      <ion-label>Karte</ion-label>
    </ion-tab-button>
  </ion-tab-bar>
</ion-tabs>
```

### ion-menu – Seitenmenü (Hamburger-Menü)
```vue
<ion-menu content-id="main-content">
  <ion-content>
    <ion-list>
      <ion-item button>Menüpunkt 1</ion-item>
      <ion-item button>Menüpunkt 2</ion-item>
    </ion-list>
  </ion-content>
</ion-menu>

<ion-router-outlet id="main-content"></ion-router-outlet>
```

### ion-segment – Segment-Controller (Umschalter innerhalb einer Seite)
```vue
<ion-segment v-model="filter">
  <ion-segment-button value="bus">
    <ion-label>Bus</ion-label>
  </ion-segment-button>
  <ion-segment-button value="bahn">
    <ion-label>Bahn</ion-label>
  </ion-segment-button>
  <ion-segment-button value="tram">
    <ion-label>Tram</ion-label>
  </ion-segment-button>
</ion-segment>
```

### ion-fab – Floating Action Button
```vue
<ion-fab vertical="bottom" horizontal="end" slot="fixed">
  <ion-fab-button>
    <ion-icon :icon="addOutline"></ion-icon>
  </ion-fab-button>
</ion-fab>
```

---

## 4. Feedback & Overlays

### ion-modal – Modal-Dialog / Bottom-Sheet
```vue
<ion-modal :is-open="istOffen">
  <ion-content class="ion-padding">
    Modal-Inhalt hier.
    <ion-button @click="istOffen = false">Schließen</ion-button>
  </ion-content>
</ion-modal>
```

### ion-alert – Bestätigungsdialog
```vue
<ion-alert
  :is-open="zeigeAlert"
  header="Bist du sicher?"
  message="Diese Aktion kann nicht rückgängig gemacht werden."
  :buttons="['Abbrechen', 'OK']"
></ion-alert>
```

### ion-toast – kurze Benachrichtigung am Bildschirmrand
```vue
<ion-toast
  :is-open="zeigeToast"
  message="Gespeichert!"
  :duration="2000"
></ion-toast>
```

### ion-action-sheet – Auswahlmenü von unten
```vue
<ion-action-sheet
  :is-open="zeigeSheet"
  header="Optionen"
  :buttons="[
    { text: 'Löschen', role: 'destructive' },
    { text: 'Abbrechen', role: 'cancel' }
  ]"
></ion-action-sheet>
```

### ion-loading – Ladeindikator-Overlay
```vue
<ion-loading :is-open="ladeVorgang" message="Bitte warten..."></ion-loading>
```

### ion-spinner – Ladeanimation (klein, inline)
```vue
<ion-spinner name="crescent"></ion-spinner>
```

### ion-progress-bar – Fortschrittsbalken
```vue
<ion-progress-bar :value="0.5"></ion-progress-bar>
```

### ion-skeleton-text – Platzhalter-Animation während Inhalte laden
```vue
<ion-item>
  <ion-skeleton-text :animated="true" style="width: 60%"></ion-skeleton-text>
</ion-item>
```

---

## 5. Scroll-Verhalten

### ion-refresher – Pull-to-refresh
```vue
<ion-content>
  <ion-refresher slot="fixed" @ionRefresh="aktualisieren($event)">
    <ion-refresher-content></ion-refresher-content>
  </ion-refresher>

  <!-- restlicher Inhalt -->
</ion-content>
```

### ion-infinite-scroll – automatisches Nachladen beim Scrollen
```vue
<ion-content>
  <!-- Liste -->

  <ion-infinite-scroll @ionInfinite="ladeMehr($event)">
    <ion-infinite-scroll-content></ion-infinite-scroll-content>
  </ion-infinite-scroll>
</ion-content>
```

---

## Empfehlung für VobiTour

Besonders relevant für das Projekt:
- **`ion-list` / `ion-item`** – Liste von Haltestellen
- **`ion-segment`** – Verkehrsmittel-Filter (Bus/Bahn/Tram)
- **`ion-modal`** – Detail-Ansicht einer Haltestelle
- **`ion-range`** – Umkreis-/Radius-Filter auf der Karte
- **`ion-refresher`** – Live-Daten (Grazer Linien, TomTom Traffic) manuell aktualisieren
- **`ion-badge`** – z. B. Verspätungs-Status auf einen Blick
