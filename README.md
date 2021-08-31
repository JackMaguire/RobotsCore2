[![Actions Status](https://github.com/JackMaguire/RobotsCore2/workflows/MacOS/badge.svg)](https://github.com/JackMaguire/RobotsCore2/actions)
[![Actions Status](https://github.com/JackMaguire/RobotsCore2/workflows/Windows/badge.svg)](https://github.com/JackMaguire/RobotsCore2/actions)
[![Actions Status](https://github.com/JackMaguire/RobotsCore2/workflows/Ubuntu/badge.svg)](https://github.com/JackMaguire/RobotsCore2/actions)
[![Actions Status](https://github.com/JackMaguire/RobotsCore2/workflows/Style/badge.svg)](https://github.com/JackMaguire/RobotsCore2/actions)
[![Actions Status](https://github.com/JackMaguire/RobotsCore2/workflows/Install/badge.svg)](https://github.com/JackMaguire/RobotsCore2/actions)
[![codecov](https://codecov.io/gh/JackMaguire/RobotsCore2/branch/master/graph/badge.svg)](https://codecov.io/gh/JackMaguire/RobotsCore2)

# Rules of the Game

![Board1.png](Board1.png)

*Fig 1*

### Board

The board is a grid of spaces.
There is one human (green),
one or more robots (blue),
and zero or more explosions (red).

### Basic Human Moves

Robots is a turn-based game.
The human has 9 directional options: up-left, up, up-right, left, center, right, down-left, down, and down-right.
Basically, the human can move to any of the spaces in the 3x3 grid centered at the human's position.

### Robot Moves

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

### Explosions

An explosion is created whenever two robots move into the same cell.
This kills the two robots and any other robot that occupies that cell in the future.
Explosions persist until the end of each round, at which point they are all removed.

### Teleports

Inevitably, the human will be cornered and will reach a point where no moves are legal.
At this point, they can teleport to a randomly selected position in the board.
This counts as a move and will trigger the robots to move in response,
possibly killing the human.

The player can accumulate up to 10 safe teleports.
This guarantees that the human will teleport to a location that will not result in their immediate death.
All teleports are "safe" until the number of safe teleports remaining is zero;
it is not possible to perform an unsafe teleport if safe teleports are available.

### Cascading

The player can trigger a cascade at any point in the game.
This will freeze the human in the current position, unable to move.
The robots will iteratively move until all of the robots are dead or until the human is dead.

If the human survives, then the player is given 1 new safe teleport for each robot killed in the cascading process.
This is really the only reason to initiate a cascade.

For example, the two positions circled in *Fig 1* are cascade-safe.
If the human moves there, then they can trigger a cascade and survive between the robots will all walk into explosions.
The first robot will die when the human walks into one of those spaces.
The second robot will die in the cascade, so the human will only be given 1 new safe teleport.

### Levels

If the humans kill all of the robots, then the level is over.
There are 66 total levels, each with more robots.
The number of robots in a given level is computed by multiplying the level number by 10.
Level 20 has 200 robots, for example.

The human begins each level at the center of the board and each robot's position is randomly generated.
Robots will **not** spawn in the same cell as the human or the same cell as each other.

If you beat level 66, you beat the game!

### Summary

Robots is a turn-based game.
For a given turn, the human has 11 moves available to it (9 in the 3x3 grid around the human, 1 for teleport, 1 to trigger a cascade).
After each human move, all robots move towards the human.
If the robots reach the human (by occupying the same cell), the game is over.
