<template>
  <ion-page>
    <ion-header>
      <ion-toolbar>
        <ion-title>Karte</ion-title>
      </ion-toolbar>
    </ion-header>

    <ion-content>
      <div id="map"></div>

      <div id="legende">
        <div class="legende-eintrag">
          <span class="legende-punkt" style="background-color: green;"></span>
          <span>Keine Verspätung</span>
        </div>
        <div class="legende-eintrag">
          <span class="legende-punkt" style="background-color: yellow;"></span>
          <span>Leichte Verspätung</span>
        </div>
        <div class="legende-eintrag">
          <span class="legende-punkt" style="background-color: red;"></span>
          <span>Starke Verspätung</span>
        </div>
      </div>
    </ion-content>
  </ion-page>
</template>

<script setup lang="ts">
import { IonPage, IonHeader, IonToolbar, IonTitle, IonContent } from '@ionic/vue';
import { onMounted, onBeforeUnmount } from 'vue';
import L from 'leaflet';
import 'leaflet/dist/leaflet.css';

let map = null;

function starteKarte() {
  map = L.map('map').setView([47.0707, 15.4395], 13);

  L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png').addTo(map);

  setTimeout(function () {
    if (map !== null) {
      map.invalidateSize();
    }
  }, 300);

  map.on('moveend', function () {
    zeigeSichtbarenBereich();
  });

  mittelPunktGraz();
  setzeAlleHaltestellen();
}

function raeumeKarteAuf() {
  if (map !== null) {
    map.remove();
  }
}

onMounted(function () {
  starteKarte();
});

onBeforeUnmount(function () {
  raeumeKarteAuf();
});

function zeigeSichtbarenBereich() {
  if (map === null) {
    return;
  }

  const bounds = map.getBounds();
  const suedwest = bounds.getSouthWest();
  const nordost = bounds.getNorthEast();

  console.log('Südwest:', suedwest.lat, suedwest.lng);
  console.log('Nordost:', nordost.lat, nordost.lng);
}


function mittelPunktGraz() {
  if (map === null) {
    return;
  }

  L.circleMarker([47.0707, 15.4395], {
    radius: 8,
    color: 'red',
    fillColor: 'black',
    fillOpacity: 1
  }).addTo(map);
}


const haltestellen = [
  { name: 'Jakominiplatz', lat: 47.07411100179385, lng: 15.435169539219155, verspaetung: 0 },
  { name: 'Hauptbahnhof', lat: 47.07265379212712, lng: 15.417991821120031, verspaetung: 3 },
  { name: 'Bulme', lat: 47.09375734863401, lng: 15.4060121295194, verspaetung: 8 }
];

function ermittleFarbe(verspaetung: number) {
  if (verspaetung < 2) {
    return 'green';
  }

  if (verspaetung < 5) {
    return 'yellow';
  }

  return 'red';
}

function setzeAlleHaltestellen() {
  if (map === null) {
    return;
  }

  for (let i = 0; i < haltestellen.length; i++) {
    const haltestelle = haltestellen[i];
    const farbe = ermittleFarbe(haltestelle.verspaetung);

    L.circleMarker([haltestelle.lat, haltestelle.lng], {
      radius: 8,
      color: farbe,
      fillColor: farbe,
      fillOpacity: 1
    }).addTo(map);
  }
}
</script>

<style scoped>
#map {
  height: 100%;
  width: 100%;
}

#legende {
  position: absolute;
  bottom: 20px;
  left: 20px;
  z-index: 1000;
  background-color: white;
  padding: 10px;
  border-radius: 8px;
  box-shadow: 0 1px 4px rgba(0, 0, 0, 0.4);
}

.legende-eintrag {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-bottom: 4px;
  color: #000;
}

.legende-punkt {
  width: 12px;
  height: 12px;
  border-radius: 50%;
  display: inline-block;
}
</style>
