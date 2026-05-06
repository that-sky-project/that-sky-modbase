# HTSkyModbase
A general mod runtime library of Sky: CotL. Developed using [HTModLoader](https://www.github.com/HTMonkeyG/HTML-Sky).

The mod is written in "Sky-style" codes.

**This mod is not an official product of Sky: Children of the Light. This mod is not approved by or associated with ThatGameCompany Inc. and Sky: Children of the Light.**

## Overview
SkyModbase provides several API functions to access Sky: CotL in-game functions or objects.

## Installation
- Compile the mod with MSVC 2022.
- Run `scripts/install.js` with NodeJS, or copy files manually:
  - Create a folder under `htmodloader/mods`, then copy `dist/that-sky-modbase.dll` and `manifest.json` to the empty folder. Copy `data/Binary.json` to `<mod_folder>/assets/Data/Manifests/Binary.json`.
  - The final directory structure should like this:
  ```
  mods/
  └─that-sky-modbase/
    ├─assets/Data/Manifests/
    │ └─Binary.json
    ├─that-sky-modbase.dll
    └─manifest.json
  ```
- Restart the game to update mods.
