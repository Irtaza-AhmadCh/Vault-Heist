# Vault Heist: The Living Vault
## Art Direction Document: Bio-Organic Neural Hive Theme

### Overview
This theme takes the subtitle "The Living Vault" literally. The game environment is a massive, living, subterranean biological hive. The data structures (Trees, Heaps) are not made of stone or metal, but are living neural networks and vascular systems that react, pulse, and "breathe" as the player interacts with them. 

---

### 1. Vertices (Nodes)
Nodes represent data points (e.g., keys in a BST, elements in a Max Heap). In this theme, they are represented as **Bioluminescent Synaptic Pods**.

#### Design Description
*   **Structure:** A circular, semi-translucent outer membrane containing a glowing, fluid-like core of nervous energy.
*   **SFML Implementation:** Rendered as a sprite with a central glowing core and a distinct outer border. The text (Key/Value) should be placed in the center using a clean, contrasting font (like white or bright cyan) with a slight dark drop-shadow for readability.

#### Color Gradients & States
*   **Default State (Active Node):**
    *   **Core Gradient:** Radial gradient from Bright Cyan (`#E0FFFF`) at the dead center, fading out to vibrant Turquoise (`#40E0D0`).
    *   **Outer Membrane:** Deep Teal (`#008080`) mixed with slight fleshy purples (`#4A0E4E`) at the very edge.
*   **Highlighted State (Selected / Traversed / Swapping):**
    *   **Core Gradient:** The core brightens intensely, shifting to an almost pure Electric Blue (`#00FFFF`) to White (`#FFFFFF`).
    *   **Animation:** A sine-wave pulsing effect applied to the alpha channel or scale in SFML to make it look like it's "firing" a synapse.
*   **Inactive State (Null / Empty Node / Extracted Heap Element):**
    *   **Core Gradient:** The light dies out, leaving a murky, dormant greyish-purple (`#554455` to `#2A202A`).
    *   **Outer Membrane:** A dried, darker maroon (`#301015`).

---

### 2. Edges (Pointers / Links)
Edges connect the vertices and represent the parent-child relationships in your data structures. In this theme, they are **Vascular Nerve Fibers**.

#### Design Description
*   **Structure:** Thick, sinewy bundles of veins and nerve tissue that twist around each other. 
*   **SFML Implementation:** Since nodes move dynamically (e.g., during AVL rotations), edges should be rendered using `sf::VertexArray` (Quads or thick Lines) textured with a repeating vein pattern, allowing them to stretch and rotate dynamically between the $(x, y)$ coordinates of parent and child nodes.

#### Color Gradients
*   **Base Tissue Gradient:** Linear gradient along the edge from Dark Crimson (`#8B0000`) to Deep Magenta (`#8B008B`), giving a fleshy, organic look.
*   **Energy Flow (Highlight):** Running through the center of the edge is a faint Turquoise line (`#40E0D0`). 
*   **Animation:** When a traversal occurs (e.g., searching for a node), you can animate a bright cyan glowing sprite moving along the length of this line, representing a nerve impulse traveling from parent to child.

---

### 3. Main Menu Background
The background sets the tone before the heist begins. It must feel imposing, ancient, and alive.

#### Design Description
*   **Environment:** A cavernous, fleshy interior resembling the inside of a massive creature or alien hive. The walls are ribbed with bone-like structures and covered in a web of dormant bio-luminescent roots.
*   **Lighting:** Dark, ambient lighting. The primary light sources are the glowing turquoise pods embedded in the walls and the central "Mother Tree" structure.
*   **Color Palette:**
    *   **Shadows:** Deep, almost pitch-black purples (`#0F0514`) and crimsons (`#1A0508`).
    *   **Midtones:** Dark, muddy organic reds and browns.
    *   **Highlights:** Piercing bioluminescent Cyan/Teal (`#00FFFF`).
*   **SFML Implementation:** A static high-resolution 2D background image. To make it feel "alive," you can overlay a subtle, low-opacity, slow-moving particle system of glowing dust or "spores" drifting upwards.

---

### 4. UI Buttons and HUD
The user interface must be clearly readable while still fitting the organic aesthetic. Avoid sharp, perfectly straight 90-degree boxes.

#### Design Description
*   **Shape:** Ovals, pill-shapes, or slightly irregular rounded rectangles. They should look like chitinous plates or hardened organic membranes resting on the screen.
*   **Border:** A thick, slightly raised border that looks like molded cartilage or hardened resin.

#### Color Gradients & States
*   **Default Button:**
    *   **Background Gradient:** Vertical linear gradient from Dark Maroon (`#3E0C17`) at the top to a muted Dark Purple (`#2D142C`) at the bottom.
    *   **Border:** Muted Teal (`#005555`).
    *   **Text:** Pale Cyan (`#E0FFFF`).
*   **Hover State:**
    *   **Background Gradient:** Lightens up slightly.
    *   **Border Glow:** The border transitions smoothly into a bright Turquoise (`#40E0D0`), emitting a soft outer glow (can be achieved with a larger, blurred sprite behind the button in SFML).
    *   **Text:** Changes to pure White (`#FFFFFF`).
*   **Click/Pressed State:**
    *   **Background Gradient:** Darkens significantly (looks like it's being pushed down into the flesh).
    *   **Border Glow:** Flashes briefly, then dims.