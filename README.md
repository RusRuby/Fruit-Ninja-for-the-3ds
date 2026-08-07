# Fruit Slice 3D — Homebrew for New 3DS XL

A simple Fruit-Ninja-style game: fruit launches upward, slide your stylus
across the touch screen to slice it, avoid the bombs, don't let fruit fall.

This repo builds automatically in the cloud via GitHub Actions — you don't
need a PC with devkitPro installed. Everything below can be done from your
phone's browser or the GitHub app.

## Requirements before you start

- **A New 3DS XL that is already CFW-modded** (e.g. via Luma3DS) **with FBI
  installed.** This project only produces homebrew software — it does not
  help you jailbreak/mod the console itself. If your 3DS isn't modded yet,
  you'll need to do that first (search "3ds homebrew launcher setup guide"
  for your firmware version) before FBI or any sideloaded CIA will run.
- A free GitHub account.
- Your 3DS and phone/PC on the same home network isn't required — FBI can
  install from any public URL.

## Step 1 — Get this code into a GitHub repo (from your phone)

1. Open github.com (or the GitHub app) and create a **new repository**,
   e.g. `fruit-slice-3ds`.
2. Use the "Add file → Upload files" option in the web UI to upload every
   file from this project, keeping the folder structure:
   - `source/main.c`
   - `Makefile`
   - `template.rsf`
   - `icon.png`
   - `banner.png`
   - `banner.wav`
   - `.github/workflows/build.yml`
3. Commit directly to the `main` branch.

## Step 2 — Let GitHub Actions build it

1. As soon as you push to `main`, go to the **Actions** tab of your repo.
2. You'll see a workflow run called "Build 3DS Game" — tap it and wait a
   few minutes (it's compiling on GitHub's servers, not your phone).
3. When it finishes with a green check, scroll down to **Artifacts** and
   download `fruit-slice-3ds.zip`. Unzip it — inside is
   `fruitninja3ds.cia`.

## Step 3 — Host the .cia somewhere with a direct URL

FBI's QR installer needs a URL that points *directly* at the `.cia` file
(not a webpage that contains a download button). Easiest options:

- **GitHub Releases** (recommended): In your repo, go to **Releases → 
  Draft a new release**, attach the `.cia` file, publish it. GitHub gives
  you a direct asset URL like:
  `https://github.com/<you>/fruit-slice-3ds/releases/download/v1/fruitninja3ds.cia`
- Or upload the `.cia` to any file host that gives a direct, non-HTML link
  (e.g. a raw GitHub file link, a Dropbox "raw=1" link, etc.)

## Step 4 — Generate the QR code

Take that direct `.cia` URL and paste it into any QR code generator
(e.g. qr-code-generator.com, or ask me and I'll generate one once you have
the URL).

## Step 5 — Install on your 3DS with FBI

1. Open **FBI** on your 3DS.
2. Press **Y** (or navigate to the QR/Remote install option — on recent
   FBI builds it's under the top menu, "Remote install" / scan QR icon).
3. Scan the QR code with your 3DS's outer camera.
4. FBI will download the `.cia` over the internet and prompt you to
   install it. Confirm, and it'll appear on your HOME Menu like any other
   game.

## Controls

- Slide your stylus across the touch screen to slice fruit
- Avoid the dark bombs — hitting one ends the run
- **START** to quit, **A** to restart after Game Over

## Notes

- The icon/banner are simple placeholder graphics generated for this
  project — swap `icon.png`/`banner.png` for your own art and re-run the
  Action any time.
- If `make cia` fails in Actions, check the build log — the most common
  cause is a `template.rsf` field devkitPro's makerom version doesn't like;
  the `.3dsx` artifact will still work fine via the Homebrew Launcher even
  if CIA packaging needs a tweak.
