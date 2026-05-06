/**
 * ThatSkyModbase installer.
 */

const ps = require("process")
  , fs = require("fs")
  , pl = require("path");

const args = ps.argv.slice(2);

if (!args.length) {
  console.log("Usage: node install.js <path>");
  console.log("  Install that-sky-modbase to the game.");
  ps.exit(1);
}

const gamePath = pl.resolve(args[0])
  , htmlModPath = pl.join(gamePath, "htmodloader/mods/that-sky-modbase")
  , distDllPath = pl.resolve(__dirname, "../dist/that-sky-modbase.dll")
  , manifestPath = pl.resolve(__dirname, "../manifest.json")
  , binaryDataPath = pl.join(__dirname, "../data/Binary.json");;

if (!fs.existsSync(distDllPath)) {
  console.error("No compiled that-sky-modbase.dll found.");
  ps.exit(1);
}

if (
  !fs.existsSync(pl.join(gamePath, "winhttp.dll"))
  || !fs.existsSync(pl.join(gamePath, "Sky.exe"))
  || !fs.existsSync(pl.join(gamePath, "htmodloader"))
  || !fs.statSync(pl.join(gamePath, "htmodloader")).isDirectory()
) {
  console.error("Not a valid Sky:CotL path with HTModLoader.");
  ps.exit(1);
}

if (
  fs.existsSync(htmlModPath)
  && fs.statSync(htmlModPath).isDirectory()
) {
  try {
    fs.rmSync(htmlModPath, { recursive: true, force: true });
  } catch (e) {
    console.error("Failed to remove previous installation.");
    ps.exit(1);
  }
}

fs.mkdirSync(htmlModPath);
fs.copyFileSync(distDllPath, pl.join(htmlModPath, "that-sky-modbase.dll"));
fs.copyFileSync(manifestPath, pl.join(htmlModPath, "manifest.json"));

fs.mkdirSync(pl.join(htmlModPath, "assets/Data/Manifests"), { recursive: true });
fs.copyFileSync(binaryDataPath, pl.join(htmlModPath, "assets/Data/Manifests/Binary.json"));

console.log("Installation finished.");
ps.exit(0);
