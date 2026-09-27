# Ionic + Vue – Lernnotizen

## 1. Installation & erstes Projekt

Voraussetzung: Node.js muss installiert sein (`node -v` zum Prüfen).

```bash
# Ionic CLI global installieren
npm install -g @ionic/cli

# Neues Projekt mit Vue erstellen (blank = leeres Starter-Template)
ionic start meineApp blank --type vue

# In den Projektordner wechseln
cd meineApp

# Dev-Server starten (Live-Reload im Browser)
ionic serve
```

`ionic serve` startet einen lokalen Vite-Dev-Server. Änderungen an `.vue`-Dateien werden per Hot-Reload sofort übernommen. Bei Änderungen am Router (`router/index.ts`) hilft manchmal ein Neustart des Servers oder ein Hard-Refresh im Browser (`Strg+Shift+R`).

---

## 2. Projektstruktur (was gehört wozu)

```
meineApp/
├── src/
│   ├── App.vue          ← Root-Komponente, NUR der Rahmen
│   ├── main.ts           ← Einstiegspunkt, bindet Vue + Ionic + Router
│   ├── router/
│   │   └── index.ts       ← definiert welche URL welche View zeigt
│   ├── theme/
│   │   └── variables.css   ← Farb-/Theme-Variablen
│   └── views/
│       ├── HomePage.vue    ← eine einzelne Seite/Ansicht
│       └── ...weitere Views
├── public/
├── capacitor.config.ts    ← Konfig für native Mobile-Builds
├── ionic.config.json
├── package.json
└── package-lock.json
```

### App.vue vs. views/*.vue

| | `App.vue` | `views/*.vue` (z. B. `HomePage.vue`) |
|---|---|---|
| Rolle | Rahmen der gesamten App | Eine einzelne Seite |
| Wie oft aktiv | Immer, wird nie ausgetauscht | Wird je nach Route ein-/ausgeblendet |
| Inhalt | Nur `<ion-app>` + `<ion-router-outlet />` | Der eigentliche Seiteninhalt (Buttons, Karten, Listen ...) |
| Vergleich | Wie eine `main()`-Funktion, die nur `runApp()` aufruft | Die eigentliche Fachlogik/der Inhalt |

**Wichtig:** `App.vue` darf man so gut wie nie anfassen. Fehler, den ich selbst gemacht habe: Buttons direkt in `App.vue` statt in `HomePage.vue` eingebaut → dabei den `<ion-router-outlet />` überschrieben → Routing hat komplett aufgehört zu funktionieren, weil der Router zwar die richtige Seite berechnet, aber keinen Platzhalter mehr hatte, wo er sie einsetzen konnte.

```vue
<!-- App.vue – bleibt IMMER so -->
<template>
  <ion-app>
    <ion-router-outlet />
  </ion-app>
</template>

<script setup lang="ts">
import { IonApp, IonRouterOutlet } from '@ionic/vue'
</script>
```

---

## 3. Router (`router/index.ts`)

Ordnet URLs (Pfade) den jeweiligen View-Komponenten zu – das Kernstück des Single-Page-App-Konzepts (keine echten Seitenwechsel, nur Austausch der Komponente im `<ion-router-outlet />`).

```ts
import { createRouter, createWebHistory } from '@ionic/vue-router';
import { RouteRecordRaw } from 'vue-router';
import HomePage from '../views/HomePage.vue'
import ButtonShowcase from '../views/ButtonShowcase.vue'

const routes: Array<RouteRecordRaw> = [
  {
    path: '/',
    redirect: '/home'      // Basis-URL leitet auf /home um
  },
  {
    path: '/home',          // die URL
    name: 'Home',           // interner Name (z.B. für Navigation per Code)
    component: HomePage     // welche Komponente angezeigt wird
  },
  {
    path: '/buttons',
    name: 'Button',
    component: ButtonShowcase
  }
]

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes
})

export default router
```

Navigation zwischen Seiten **innerhalb** der App (ohne vollen Page-Reload):

```vue
<ion-button router-link="/buttons">Zu den Buttons</ion-button>
<!-- oder mit Vue Router direkt: -->
<router-link to="/buttons">Zu den Buttons</router-link>
```

`main.ts` muss den Router registrieren, sonst funktioniert Routing gar nicht:

```ts
const app = createApp(App)
  .use(IonicVue)
  .use(router)

router.isReady().then(() => {
  app.mount('#app')
})
```

---

## 4. Aufbau einer View-Datei (z. B. `HomePage.vue`)

```vue
<template>
  <ion-page>
    <ion-header :translucent="true">
      <ion-toolbar>
        <ion-title>Home</ion-title>
      </ion-toolbar>
    </ion-header>

    <ion-content :fullscreen="true">
      <!-- Inhalt der Seite -->
    </ion-content>
  </ion-page>
</template>

<script setup lang="ts">
import { IonContent, IonHeader, IonPage, IonTitle, IonToolbar } from '@ionic/vue';
</script>

<style scoped>
/* nur für diese Komponente gültig, nicht global */
</style>
```

- **`<ion-page>`**: Wrapper für jede einzelne Seite, kümmert sich um Layout & Übergangsanimationen. Jede View, die über den Router geladen wird, braucht das.
- **`<ion-header>`**: fester Kopfbereich (bleibt beim Scrollen oben).
- **`<ion-toolbar>`**: die Leiste selbst, kann Titel, Buttons, Searchbar enthalten.
- **`<ion-content>`**: scrollbarer Hauptbereich, hier kommt der eigentliche Inhalt rein.
- **`<script setup>`**: alle im Template verwendeten Ionic-Komponenten müssen hier importiert werden, sonst werden sie nicht erkannt.
- **`<style scoped>`**: `scoped` = Styles gelten nur für diese Komponente, nicht global.

---

## 5. ion-button – alle Varianten

```vue
<!-- fill -->
<ion-button fill="solid">Solid</ion-button>
<ion-button fill="outline">Outline</ion-button>
<ion-button fill="clear">Clear</ion-button>

<!-- color -->
<ion-button color="primary">Primary</ion-button>
<ion-button color="secondary">Secondary</ion-button>
<ion-button color="danger">Danger</ion-button>
<!-- weitere: tertiary, success, warning, light, medium, dark -->

<!-- size -->
<ion-button size="small">Small</ion-button>
<ion-button size="large">Large</ion-button>

<!-- shape -->
<ion-button shape="round">Round</ion-button>

<!-- expand: volle Breite -->
<ion-button expand="block">Block</ion-button>
<ion-button expand="full">Full</ion-button>

<!-- strong: fetter Text -->
<ion-button strong="true">Strong</ion-button>

<!-- disabled -->
<ion-button disabled="true">Disabled</ion-button>

<!-- mit Icon -->
<ion-button>
  <ion-icon slot="start" :icon="heartOutline"></ion-icon>
  Text
</ion-button>

<!-- nur Icon -->
<ion-button shape="round">
  <ion-icon slot="icon-only" :icon="heartOutline"></ion-icon>
</ion-button>

<!-- Navigation -->
<ion-button router-link="/buttons">Navigieren</ion-button>
```

---

## 6. ion-header / ion-title – Animations-Effekte

Gelten größtenteils **nur im iOS-Modus** (im `md`/Android-Modus werden sie meist ignoriert).

| Effekt | Code | Wirkung |
|---|---|---|
| Translucent | `<ion-header :translucent="true">` + `<ion-content :fullscreen="true">` | Content scrollt sichtbar (weichgezeichnet) durch den Header |
| Condense (großer Titel) | zweiter `<ion-header collapse="condense">` innerhalb von `ion-content`, `<ion-title size="large">` | Großer Titel kollabiert beim Scrollen zum kleinen Titel |
| Fade | `<ion-header collapse="fade">` | Header-Hintergrund/Rand ist zunächst unsichtbar, blendet beim Scrollen ein |
| Kein Rand | `<ion-header class="ion-no-border">` | Entfernt Trennlinie/Schatten unten am Header |
| Mehrere Toolbars | zwei `<ion-toolbar>` im selben `<ion-header>` | z. B. Titel + Searchbar untereinander |
| Buttons beim Collapse ausblenden | `<ion-buttons collapse="true">` | Buttons nur im expandierten Zustand sichtbar |
| Title-Größen | `<ion-title size="small">`, ohne Attribut (default), `size="large"` | verschiedene Titel-Schriftgrößen |

Merksatz zum Doppel-Header-Muster im Standard-Template:
- Der **äußere** `ion-header` (mit `translucent`) = kollabierter/kleiner Zustand
- Der **innere** `ion-header` (mit `collapse="condense"`, innerhalb von `ion-content`) = expandierter/großer Zustand

---

## 7. Code auslagern: Composables (Vue-Äquivalent zu Header/CPP)

Im Gegensatz zu C++ gibt es keine strikte Trennung Deklaration/Implementierung – eine `.ts`-Datei ist beides in einem.

```ts
// src/composables/useCounter.ts
import { ref } from 'vue'

export function useCounter() {
  const count = ref(0)
  function increment() { count.value++ }
  return { count, increment }
}
```

```vue
<!-- Verwendung in einer beliebigen View -->
<script setup lang="ts">
import { useCounter } from '../composables/useCounter'
const { count, increment } = useCounter()
</script>
```

Für nicht-reaktive Helper-Funktionen (ohne Vue-spezifisches `ref`) reicht eine normale `.ts`-Datei, z. B. `src/utils/format.ts`.

---

## 8. .gitignore – was nicht committet wird

```gitignore
node_modules/
dist/
www/
.capacitor/
ios/App/Pods/
ios/App/build/
android/app/build/
android/.gradle/
android/local.properties
.env
.env.local
.env.*.local
.vscode/settings.json
.DS_Store
*.log
.vite/
```

Wird von der Ionic-CLI bei `ionic start` meist schon automatisch korrekt angelegt. Alles andere (Konfig-Dateien, `package.json`/`package-lock.json`, Quellcode, Tests) gehört normalerweise ins Repo.

---

## 9. Mobile Build (Android/iOS) über Capacitor

Ionic-Webcode wird über **Capacitor** in eine echte native App verpackt:

```bash
ionic build                       # Web-App bauen
ionic capacitor add android       # Android-Plattform hinzufügen (einmalig)
ionic capacitor add ios           # iOS-Plattform hinzufügen (einmalig)
ionic capacitor sync              # Web-Build in native Projekte kopieren
ionic capacitor open android      # in Android Studio öffnen
ionic capacitor open ios          # in Xcode öffnen
```

Für nativen Zugriff (GPS, Kamera, Push-Notifications etc.) gibt es Capacitor-Plugins, während man weiterhin in Vue programmiert.

**Einordnung für VobiTour:** Ionic = "einmal schreiben, überall laufen lassen", etwas weniger native Performance. Qt/QML = näher an "echt nativ", dafür mehr Aufwand pro Plattform.
