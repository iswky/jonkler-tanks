### TODO list:
#### from main reqs
- [x] Main menu
- [x] About menu
- [x] help menu
- [x] saving game after any action
- [x] score board
- [x] bots
	- [x] difficulties for bots
#### yeah i like these 1s
- [x] map generating with perlin 1d noise
	- [x] mb do some small improvements for map generating cuz of strange differences in resolutions 
	- [x] main things
- [ ] create a sound effects (like for everything)
- [x] create renderer 
	- [x]  z-pos sorting (dont need that)
	- [x] custom struct for easier rendering
- [x] debug info'es
- [x] add info bar at the bottom of the screen
#### funny stuff
- [ ] clash royale emotes



## dependencies
### 1. Building tools
`cmake make gcc`
### 2. SDL libs
`sdl2 sdl2_image sdl2_mixer sdl2_ttf` 
## building
```
mkdir build && cd build
cmake ..
make -j4
```

## running
Just run the 'JonklerTanks' executable
