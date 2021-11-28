# Tanks game

Client-server multiplayer game. Uses TCP/IP for connection.

Each round up to 10 players connected to the server spawn in a randomly generated maze. Each one is controlling a tank. All tanks can shoot, the type of projectile depends on the current game mode and player powerup. Projectiles bounce off the walls, but their properties can be very different depending on the type. Tanks can be destroyed with a single shot. The only one who is alive at the end wins the round. After that a new round begins, and the game continues.

## Files

### Client

The [executable file](https://github.com/Petr1Furious/tanks-game/blob/main/Tank%20maze%20c/x64/Release/Tank%20maze%20c.exe) requires the Resources folder and files msvcp140.dll, msvcp140_1.dll, vcruntime140.dll and vcruntime140_1.dll to run. All of them are located [here](https://github.com/Petr1Furious/tanks-game/tree/main/Tank%20maze%20c/x64/Release).

After first launch, the file "option.txt" will be created in the same directory. It is possible to change nickname, password and server password there.

### Server

The [executable file](https://github.com/Petr1Furious/tanks-game/blob/main/Tank%20maze%20s/x64/Release/Tank%20maze%20s.exe) requires files msvcp140.dll, msvcp140_1.dll, vcruntime140.dll and vcruntime140_1.dll to run. All of them are located [here](https://github.com/Petr1Furious/tanks-game/tree/main/Tank%20maze%20s/x64/Release).

## Usage

### Startup

First, ask the host to launch the server. The server has a fixed port: 13111. Port forwarding on this port can be done to be able to play outside of the local network.

At startup, the client will automatically try to connect to 127.0.0.1:13111 (localhost, port 13111). If the server is not being run locally, it will fail after 3 seconds and show the error message.

After that, type "/connect ip_address" in the blank white window, where instead of ip_address put the target server IP address (for example, "/connect 127.0.0.1").

When a player connects to the server, it outputs a message containing player information. After all the players connect to the server, type "start" in the cmd window.


### Client controls

2 types of in-game controls are possible:
- Mouse controls: drag mouse to direct the tank, left click to shoot; left click to switch to this controls
- Keyboard controls: WASD to move, space to shoot; space to switch to this controls

Other controls:
- **T** - open the chat
- **F11** - switch to fullscreen mode
- **TAB** - autocomplete the command

Commands:
- **/connect <IP address>** - connect to the given IP address
- **/disconnect** - disconnect from the server
- **/start** - start the game
- **/stop** - stop the server
- **/op <nickname>** - give operator permissions to the player
The list of the other commands can be accessed by typing **/** in the chat and pressing TAB. Some of them cannot be executed by a player without the operator permissions.

### Server

Server can also execute commands (for example, it is the only way to give the first player operator permissions, or to start a game without such player). A caommand should not contain **/** in front of it.

## Example

[Video example](https://drive.google.com/file/d/1Dyx0IK2OmFkFUrHUUvgWN0mZ2b6h16Oy/view?usp=sharing). In the video example both clients are launched from one PC.
