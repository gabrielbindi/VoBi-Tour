```bash
sudo apt update
sudo apt install nodejs npm

```


* **Arch Linux / Manjaro:**
```bash
sudo pacman -S nodejs npm

```

2. **Installation prüfen:**
Führe danach im Terminal folgendes aus, um zu prüfen, ob npm gefunden wird:
```bash
node -v
npm -v

```

3. **Abhängigkeiten installieren:**
Sobald `npm` installiert ist, kannst du deinen Befehl im Projektverzeichnis ausführen:
```bash
npm install

```


4. Neueste Ionic CLI installieren

Installiere das aktuelle `@ionic/cli` Paket global:

```bash
npm install -g @ionic/cli

```

### 2. Projekt starten

Verwende nun die globale CLI oder führe den Befehl über `npx` mit dem richtigen Paketnamen aus:

```bash
ionic serve

```

*(Alternativ ohne globale Installation: `npx @ionic/cli serve`)*

### 3. Alternativ: Direct Vite-Dev-Server nutzen

Da dein Projekt mit Vite aufgesetzt ist (siehe `vite.config.ts`), kannst du es in Ionic/Vue-Projekten meistens auch direkt über das npm-Script starten:

```bash
npm run dev

```
