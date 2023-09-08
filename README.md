# Snake game - Linux terminal version

This is a Snake game that runs on Linux terminals. The game is written in C++, and it is a practice project for me to learn C++.

![](./img/show.gif)

## Dependencies

This Snake game depends on the ncurses library, make sure you have it installed on your system. You can install it using the following command:

```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```

## How to build

To build and run this Snake game, the following steps need to be followed:

1. Clone the project locally:

```bash
git clone https://github.com/twowind/snake.git
cd snake
```

2. build

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . && ./main
```

## Controls

Use the arrow keys or the h, j, k and l keys to move the snake left, down, up and right.

## License
This project is under the MIT license, which means you are free to use, modify and distribute this project.