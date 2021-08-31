[![Actions Status](https://github.com/JackMaguire/RobotsCore2/workflows/MacOS/badge.svg)](https://github.com/JackMaguire/RobotsCore2/actions)
[![Actions Status](https://github.com/JackMaguire/RobotsCore2/workflows/Windows/badge.svg)](https://github.com/JackMaguire/RobotsCore2/actions)
[![Actions Status](https://github.com/JackMaguire/RobotsCore2/workflows/Ubuntu/badge.svg)](https://github.com/JackMaguire/RobotsCore2/actions)
[![Actions Status](https://github.com/JackMaguire/RobotsCore2/workflows/Style/badge.svg)](https://github.com/JackMaguire/RobotsCore2/actions)
[![Actions Status](https://github.com/JackMaguire/RobotsCore2/workflows/Install/badge.svg)](https://github.com/JackMaguire/RobotsCore2/actions)
[![codecov](https://codecov.io/gh/JackMaguire/RobotsCore2/branch/master/graph/badge.svg)](https://codecov.io/gh/JackMaguire/RobotsCore2)

# Rules of the Game

![Board1.png](Board1.png)

*Fig 1*

#### Board

The board is a grid of spaces.
There is one human (green),
one or more robots (blue),
and zero or more explosions (red).

#### Basic Human Moves

Robots is a turn-based game.
The human has 9 directional options: up-left, up, up-right, left, center, right, down-left, down, and down-right.
Basically, the human can move to any of the spaces in the 3x3 grid centered at the human's position.

#### Robot Moves

After every move the human makes, each robot takes a deterministic step in the direction of the human.
