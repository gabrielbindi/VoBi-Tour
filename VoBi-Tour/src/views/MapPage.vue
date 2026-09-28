<template>
  <ion-page>
    <ion-header>
      <ion-toolbar>
        <ion-title>Karte</ion-title>
      </ion-toolbar>
    </ion-header>

    <ion-content>
      <div id="map"></div>
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
</script>

<style scoped>
#map {
  height: 100%;
  width: 100%;
}
</style>
