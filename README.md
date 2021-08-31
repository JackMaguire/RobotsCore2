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

Like the human, each robot can move to any of the cells in the 3x3 grid centered at the robot.
After every move the human makes, each robot takes a deterministic step in the direction of the human:

```c++
// For robot with index r
Position & pos = robot_positions_[ r ];

if(      human_position_.x < pos.x ) pos.x -= 1;
else if( human_position_.x > pos.x ) pos.x += 1;

if(      human_position_.y < pos.y ) pos.y -= 1;
else if( human_position_.y > pos.y ) pos.y += 1;
```

#### Explosions

An explosion is created whenever two robots move into the same cell.
This kills the two robots and any other robot that occupies that cell in the future.
Explosions persist until the end of each round, at which point they are all removed.

#### Teleports

Inevitably, the human will be cornered and will reach a point where no moves are legal.
At this point, they can teleport to a randomly selected position in the board.
This counts as a move and will trigger the robots to move in response,
possibly killing the human.

The player can accumulate up to 10 safe teleports.
This guarantees that the human will teleport to a location that will not result in their immediate death.
All teleports are "safe" until the number of safe teleports remaining is zero;
it is not possible to perform an unsafe teleport if safe teleports are available.
