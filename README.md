# Boids Computer Graphics Project

## Final Report - CG Project: Boids (Underwater World)

### Team Members
- **Julia Osińska**
- **Robert Piłat** ([GitHub](https://github.com/luminoksik))
- **Agata Zaparucha** ([GitHub](https://github.com/agataaga))

---

## Task Division

### Agata Zaparucha
#### Tasks
- **Boids Algorithm**  
  Implemented cohesion, alignment, and separation rules. Fish in the same group swim together in shoals, influencing each other. Boids are confined within a cuboid space with obstacle columns they can collide with.

- **OBB Collision Detection**  
  Each fish has its own hitbox, transformed based on position and rotation. A Separating Axis Theorem (SAT) check determines collisions—if two objects' projections don’t overlap on any axis, they don’t collide. Boids react accordingly to collisions with other boids or obstacles.

---

### Robert Piłat
#### Tasks
- **Environment**  
  - Terrain (sand) generated using Perlin noise, then smoothed with S-curve and linear interpolation.
  - Normal vectors calculated for each terrain point.
  - Skybox implemented using a mix of downloaded and AI-generated textures (processed with Python scripts for formatting and cutting).

- **Shadow Mapping**  
  Shadows implemented for terrain and columns. A depth map is created from the light source's perspective and saved as a texture, later used in final scene rendering.

---

### Julia Osińska
#### Tasks
- **Normal Mapping**  
  Normal mapping implemented for fish models (scales) and stone-like columns to enhance realism using the Tangent Space Normal Mapping method.

- **Interactivity**  
  - Interactive elements added using the ImGui library.
  - A control panel allows the user to adjust parameters such as:
    - Number of groups and boids per group
    - Boids' max speed, alignment weight, etc.
    - Enabling/disabling shadow and normal mapping
  - Added functionality to attract or disperse boids using the right mouse button (click or hold). Mode switching is available via the control panel.

- **Visual Composition**  
  - Boids represented as fish models loaded via the Assimp library.
  - Shader features include texturing, Phong lighting, and normal mapping.
  - The same techniques are applied to obstacle columns for a cohesive visual style.

---

This project aims to create a visually appealing and interactive underwater simulation using the Boids algorithm. The environment, lighting, and interactivity contribute to an immersive experience.
