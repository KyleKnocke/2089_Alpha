# Sim.Orbits Module

Authoritative orbital and relative motion simulation implemented in double precision. Use RK4 or Verlet integrators to keep drift low and export transforms that gameplay systems consume each tick.

Key responsibilities:

- Model patched-conic trajectories for ships, EVA actors, and debris.
- Provide local gravity volumes for transitions between ship, lunar surface, and open space.
- Emit deterministic transforms so FloatingOrigin can rebase without desync.
