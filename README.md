# 2089 Alpha Project Structure

This repository holds the Unreal Engine 5 "2089 Alpha" prototype. The layout mirrors the core gameplay, simulation, and content modules we discussed for the vertical slice.

## High-level layout

- `Source/`
  - C++ modules broken into small, testable plugins that map to the primary gameplay systems.
- `Content/`
  - Art, data assets, and graybox level tiles.
- `Config/`
  - Default project configuration and plugin settings.

The goal is to keep systems decoupled so the double-precision simulation can drive authoritative transforms while GAS, Mass AI, and CommonUI focus on player experience.

## Module overview

### Sim
`Source/Sim` contains deterministic, double-precision gameplay simulation code that remains independent of the engine's Chaos physics.

- **Orbits** – Orbital mechanics, relative motion, and transform export (RK4 / Verlet). Feeds the rest of the runtime.
- **FloatingOrigin** – Origin rebasing, sublevel streaming, and precision hand-off for interior/exterior transitions.

### Gameplay
`Source/Gameplay` hosts player-facing systems that consume simulation data.

- **EVA** – Custom `UCharacterMovementComponent` derivatives for RCS thrusters, tether traversal, and hand-rail grabs.
- **Signals** – Sensor stealth gameplay for acoustic/EM/thermal signatures.

### RPG
`Source/RPG/GAS` packages GAS attributes, abilities, and effects that model suit resources and player actions.

### AI
`Source/AI/Mass` focuses on MassAI agents for swarms, crowds, and autonomous probes.

### UI
`Source/UI/Common` holds CommonUI widgets for the diegetic HUD and tablet interactions.

## Content overview

- `Content/DataAssets/` – Suit archetypes, ship modules, anomaly definitions, and ability tunables (stored as `PrimaryDataAsset`).
- `Content/GrayboxTiles/`
  - `DerelictFreighter/` – Cislunar cargo hull interior tiles.
  - `UnderIceShaft/` – Europan ocean shaft layout.
  - `LavaTube/` – Lunar lava-tube segments.

## Build & plugin notes

1. Create a C++ UE5 project (not Blueprint-only) so you can extend `CharacterMovementComponent` and author precision-safe modules.
2. Enable the following plugins:
   - GameplayAbilities (GAS)
   - Mass AI
   - CommonUI
   - OpenXR (optional for future VR experiments)
   - Niagara
   - Control Rig
3. Retain GAS server-authoritative patterns even for single-player to stay multiplayer-ready.

## Future expansion

- Networking hooks for coop, even if initially offline.
- Audio-driven stealth feedback via MetaSounds tuned to hull thickness/medium.
- Performance guardrails: profile Niagara/translucency, author Nanite LODs, and keep post volumes light.

This structure should make it straightforward to layer in new systems without entangling the deterministic simulation with presentation code.
