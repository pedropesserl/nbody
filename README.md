# nbody

This is a simulation of the n-body problem in physics. It is written in C and it uses
the library Raylib for the GUI.

<img src="example.gif" width="350">

## Quick Start

### Install [Raylib](https://raylib.com)

#### On Linux

https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux

#### On Windows

https://raysan5.itch.io/raylib/download/eyJpZCI6ODUzMzEsImV4cGlyZXMiOjE3MDM4MjE2MDF9%2eeHtu8IQLjDZOCRSj1JbuoS18Fbg%3d

### Compile

```bash
$ git clone https://github.com/pedropesserl/nbody.git
$ make
```

## Usage

- `$ ./nbody` - Make an empty space and fill it with bodies by clicking the screen.

- `$ ./nbody -n <number_of_bodies>` - Pre-fill the space with bodies by writing the data about them in stdin: mass, position (x), position (y), velocity (x), velocity (y).
