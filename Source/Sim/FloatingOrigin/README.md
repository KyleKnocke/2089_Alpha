# Sim.FloatingOrigin Module

Manages precision by rebasing the world origin and streaming sublevels for ship interiors, EVA exterior shells, and planetary surfaces.

Responsibilities:

- Maintain tight coupling with Sim.Orbits to know when to shift reference frames.
- Stream interior/exterior/lunar tiles based on proximity.
- Coordinate tether/hardline attachments so actors remain stable during rebases.
