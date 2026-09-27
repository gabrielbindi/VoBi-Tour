<template>
  <ion-page>
    <ion-header>
      <ion-toolbar>
        <ion-title>Button Showcase</ion-title>
      </ion-toolbar>
    </ion-header>

    <ion-content class="ion-padding">

      <!-- FILL: solid, outline, clear -->
      <h2>Fill</h2>
      <ion-button fill="solid">Solid (Standard)</ion-button>
      <ion-button fill="outline">Outline</ion-button>
      <ion-button fill="clear">Clear</ion-button>

      <!-- COLOR: die Standard-Farbpalette -->
      <h2>Color</h2>
      <ion-button color="primary">Primary</ion-button>
      <ion-button color="secondary">Secondary</ion-button>
      <ion-button color="tertiary">Tertiary</ion-button>
      <ion-button color="success">Success</ion-button>
      <ion-button color="warning">Warning</ion-button>
      <ion-button color="danger">Danger</ion-button>
      <ion-button color="light">Light</ion-button>
      <ion-button color="medium">Medium</ion-button>
      <ion-button color="dark">Dark</ion-button>

      <!-- SIZE: small, default, large -->
      <h2>Size</h2>
      <ion-button size="small">Small</ion-button>
      <ion-button size="default">Default</ion-button>
      <ion-button size="large">Large</ion-button>

      <!-- SHAPE: round -->
      <h2>Shape</h2>
      <ion-button shape="round">Round</ion-button>
      <ion-button shape="round" fill="outline">Round Outline</ion-button>

      <!-- EXPAND: block, full -->
      <h2>Expand</h2>
      <ion-button expand="block">Block (volle Breite, mit Rand)</ion-button>
      <ion-button expand="full">Full (volle Breite, randlos)</ion-button>

      <!-- STRONG -->
      <h2>Strong (fetter Text)</h2>
      <ion-button strong="true">Strong Text</ion-button>

      <!-- DISABLED -->
      <h2>Disabled</h2>
      <ion-button disabled="true">Disabled</ion-button>

      <!-- MIT ICON -->
      <h2>Mit Icon</h2>
      <ion-button>
        <ion-icon slot="start" :icon="heartOutline"></ion-icon>
        Icon Start
      </ion-button>
      <ion-button>
        Icon End
        <ion-icon slot="end" :icon="arrowForwardOutline"></ion-icon>
      </ion-button>

      <!-- NUR ICON -->
      <h2>Nur Icon (icon-only)</h2>
      <ion-button shape="round">
        <ion-icon slot="icon-only" :icon="heartOutline"></ion-icon>
      </ion-button>
      <ion-button fill="outline" shape="round">
        <ion-icon slot="icon-only" :icon="settingsOutline"></ion-icon>
      </ion-button>

      <!-- KOMBINATIONEN -->
      <h2>Kombinationen (mehrere Attribute gleichzeitig)</h2>
      <ion-button color="success" fill="outline" size="large" shape="round">
        Kombi 1
      </ion-button>
      <ion-button color="danger" expand="block" strong="true">
        Kombi 2
      </ion-button>

      <!-- CLICK-HANDLER -->
      <h2>Mit Click-Handler</h2>
      <ion-button @click="onClick">Klick zählt: {{ count }}</ion-button>

    </ion-content>
  </ion-page>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import {
  IonPage,
  IonHeader,
  IonToolbar,
  IonTitle,
  IonContent,
  IonButton,
  IonIcon
} from '@ionic/vue'
import {
  heartOutline,
  arrowForwardOutline,
  settingsOutline
} from 'ionicons/icons'

const count = ref(0)
function onClick() {
  count.value++
}
</script>

<style scoped>
h2 {
  margin-top: 24px;
  font-size: 16px;
  opacity: 0.7;
}
</style>
