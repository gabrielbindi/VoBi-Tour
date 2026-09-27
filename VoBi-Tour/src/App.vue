<template>
  <ion-page>
    <ion-content class="ion-padding">
      <ion-button @click="meineFunktion">Klick mich</ion-button>
      <ion-button color="danger" fill="outline">Zweiter Button</ion-button>
    </ion-content>
  </ion-page>
</template>

<script setup>
import { IonPage, IonContent, IonButton } from '@ionic/vue'

function meineFunktion() {
  console.log('Button geklickt!')
}
</script>
