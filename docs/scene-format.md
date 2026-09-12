# The `.rts` scene format

Retsu reads scenes from a plain-text file with a small INI-like grammar. The
format is meant to be written and edited by hand, and parsed by a few hundred
lines of C++ with no external dependency.

A French version of this document, predating the reflection support, is kept as
`scene-format-fr.pdf`.

---

## Structure

A scene is a list of **sections**. Each section opens with a type name in
square brackets and runs until the next section or the end of the file. Inside
a section, each line is a **property**, written `name = value`.

```ini
[Camera]
position = 0, 1, -5
lookAt = 0, 0, 0
fov = 60
```

Section order does not matter, except that only the first `[Camera]` is used.
The same primitive or light type may appear as many times as you like.

## Value types

| Type | Form | Example |
|---|---|---|
| Number | integer or decimal | `2`, `3.14`, `-0.5` |
| Vector | three numbers, comma separated | `1, 2, 3` |
| Colour | a vector, each component in `[0, 1]` | `1, 0, 0` is red |
| Boolean | `true` or `false` | `true` |

Whitespace around commas and around `=` is ignored. A line whose first
non-blank character is `#` is a comment, as is anything after a `#` on a
property line.

```ini
color = 0.8, 0.8, 0.8    # light grey
```

## A note on coordinates

The world is right-handed with `+y` up. The camera basis, however, is built as
`u = up × (−direction)`, which means that **with a camera looking toward `+z`,
increasing `x` moves an object LEFT across the frame**.

This trips up anyone composing a scene by intuition, so it is worth stating
plainly rather than leaving to discovery. The bundled scenes are written with
the convention in mind and say so in their comments.

## Camera

Exactly one per scene.

```ini
[Camera]
position = 0, 1, -5          # eye position
lookAt = 0, 0, 0             # point the camera aims at
up = 0, 1, 0                 # world up, normally 0,1,0
fov = 60                     # vertical field of view, degrees
aspectRatio = 1.77778        # width / height, 16:9 = 1.77778
resolution = 1920, 1080      # output size in pixels
```

`resolution` sets the rendered image size. `aspectRatio` controls the shape of
the view plane; if it disagrees with the resolution, the image is stretched.

## Primitives

Every primitive accepts two shared properties:

| Property | Type | Default | Meaning |
|---|---|---|---|
| `color` | colour | varies | Base surface colour |
| `reflectivity` | number `[0, 1]` | `0` | Share of the surface's colour taken from its mirror reflection |

`reflectivity = 0` is a purely diffuse surface. `1` is a perfect mirror. The
default of `0` means a scene written before reflection existed renders
identically today.

Reflection is recursive and bounded by `--depth` on the command line, which
defaults to 5.

### Sphere

```ini
[Sphere]
center = 0, 0, 0
radius = 1
color = 1, 0, 0
reflectivity = 0.4
```

### Plane

Infinite, defined by a point on it and its normal.

```ini
[Plane]
point = 0, -1, 0             # any point lying on the plane
normal = 0, 1, 0             # perpendicular, need not be normalised
color = 0.5, 0.5, 0.5
```

Enclosing a scene in six planes is the usual way to make reflective surfaces
interesting: a mirror facing an open scene reflects mostly background, which is
black.

### Cylinder

Finite and capped at both ends.

```ini
[Cylinder]
basePoint = 0, -1, 0         # centre of the bottom cap
axis = 0, 1, 0               # direction from base toward top
radius = 0.5
height = 2
color = 0, 1, 0
```

### Cone

Finite, with its tip at `apex` and opening along `axis`.

```ini
[Cone]
apex = 0, 1, 0               # the tip
axis = 0, -1, 0              # direction the cone opens toward
angle = 30                   # half-angle at the apex, degrees
height = 2                   # distance from apex to base
color = 1, 1, 0
```

## Lights

At least one is needed, or the scene renders as silhouettes against the ambient
floor.

### AmbientLight

Uniform, directionless, casts no shadow. Use it to lift the shadows, not to
light the scene.

```ini
[AmbientLight]
intensity = 0.2              # 0.0 to 1.0
color = 1, 1, 1
```

### DirectionalLight

Infinitely distant, so its rays are parallel and its shadows never converge.

```ini
[DirectionalLight]
direction = 1, -1, 0         # the direction the light TRAVELS
intensity = 0.7
color = 1, 1, 1
```

`direction` describes where the light is going, not where it comes from. A
value of `0, -1, 0` shines straight down.

### PointLight

Radiates from a position, so shadows spread with distance.

```ini
[PointLight]
position = 2, 4, 0
intensity = 0.8
color = 1, 0.8, 0.5
```

Do not place a point light exactly in the plane of a surface. A shadow ray
aimed at it then grazes that surface at a distance equal to the light's own, and
the "is something in the way" comparison becomes a coin flip decided by
rounding. The result is a fine speckle across everything the light touches.
Move the source a little clear of the geometry.

## Complete example

```ini
# A red sphere on a reflective floor, lit from the front left.

[Camera]
position = 0, 1.2, -6
lookAt = 0, 0.2, 0
up = 0, 1, 0
fov = 55
aspectRatio = 1.77778
resolution = 1600, 900

[Plane]
point = 0, -1, 0
normal = 0, 1, 0
color = 0.45, 0.46, 0.50
reflectivity = 0.35

[Plane]
point = 0, 0, 6
normal = 0, 0, -1
color = 0.55, 0.48, 0.42

[Sphere]
center = 0, 0, 0
radius = 1
color = 0.85, 0.25, 0.22

[Sphere]
center = 2.2, 0, 1.5
radius = 1
color = 0.92, 0.93, 0.96
reflectivity = 0.8

[Cylinder]
basePoint = -2.5, -1, 0.5
axis = 0, 1, 0
radius = 0.35
height = 2
color = 0.30, 0.52, 0.72
reflectivity = 0.2

[AmbientLight]
intensity = 0.22
color = 0.92, 0.94, 1.0

[PointLight]
position = -4, 5, -3
intensity = 0.85
color = 1.0, 0.94, 0.84

[DirectionalLight]
direction = 0.3, -1, 0.4
intensity = 0.2
color = 1, 1, 1
```

## Why this format

The obvious alternative was libconfig++, which the project could have taken as a
dependency. Writing the parser instead was the better trade for four reasons:

**No dependency.** The whole renderer builds with a compiler and nothing else.
Optional SFML and OpenMP can both be switched off and the build stays clean.

**The grammar is the documentation.** Sections and `key = value` need no
explanation. Anyone can open a scene file and change a number.

**The parser is small.** A tokenizer and a section reader, a few hundred lines,
easy to follow and easy to extend.

**Extending it is local.** A new primitive means one class implementing
`IPrimitive` and one branch in `PrimitiveFactory`. The parser itself does not
change, because it only ever produces sections of named properties, and the
factories decide what those mean.

## Adding a property

Properties are read through `SceneSection`, which returns a typed value with a
fallback:

```cpp
float reflectivity = section.getFloat("reflectivity", 0.0f);
Vector3 center     = section.getVector3("center");
Color color        = section.getColor("color", Color(1.0f, 0.0f, 0.0f));
```

Because every getter takes a default, adding a property never breaks an existing
scene file. That is how `reflectivity` was introduced.
