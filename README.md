# nbody

This is a simulation of the n-body problem in physics. It is written in C and it uses
the library Raylib for the GUI.

<img src="example.gif" width="700">

## Quick Start

### Install [Raylib](https://raylib.com)

- On Linux: https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux
- For Web: https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5)

### Clone repo

```bash
$ git clone https://github.com/pedropesserl/nbody.git
```

### Compile 

Compilation options: `nbody` (default), `web`, `all` (both)

```bash
$ make [compilation_option]
```

## Usage

- `$ ./nbody` - Make an empty space and fill it with bodies by clicking the screen.

- `$ ./nbody -n <number_of_bodies>` - Pre-fill the space with bodies by writing the data about them in stdin: mass, density, position (x), position (y), velocity (x), velocity (y).

Note: Mass and density must be greater than zero. The radius of each body is calculated as the cube root of its volume (mass/density), to emulate the square-cube law. 

## Controls

- Mouse Left: add planet
- Mouse Right (drag): move through space
- Mouse Wheel: zoom
- `␣` Play/pause simulation
- `a` Toggle arrows
- `t` Toggle trails
- `←` Rewind
- `→` Fast forward

## Sources

- https://www.raylib.com/examples.html
- https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional
- https://youtube.com/@TsodingDaily
