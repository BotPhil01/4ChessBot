# 4ChessBot
---
With the popularity of Chess nowadays the servers are filled with even more players. Lichess and Chess.com have experienced a massive increase in the number of active users, yet this is not true for all areas of Chess, especially variants. As a result of the lower popularity and interest in 4-Chess, this application will allow users to play against a 4-Chess bot. This will also allow me to make a chess engine and learn some basic frontend and web app development. If you decide to check this out I thank you for taking the time.

# Reporting Issues
Report issues or code suggestions by creating an issue on github.

# How to use this project? 

## Safe
### Docker
This project is accessible as a docker container on dockerhub. See https://hub.docker.com/r/botphil01/4chessbot

Alternatively you can pull this using the command:
`docker pull botphil01/4chessbot:v1.0.0`
To run the image use the command:
`docker run -d -p 127.0.0.1:3000:3000 botphil01/4chessbot:v1.0.0`
and then navigate to http://127.0.0.1:3000

## UNSAFE
***WARNING!!! THIS CODE USES C++ AND COULD CAUSE ISSUES IF RAN OUTSIDE OF A VM OR DOCKER CONTAINER. CODE IS MAINLY CHECKED WITH VALGRIND HOWEVER MEMORY SAFETY CANNOT BE GUARANTEED. THE MAINTAINER OF THIS REPOSITORY TAKES NO RESPONSIBILITY FOR THE EFFECTS OF THIS CODE ON RAN ON OTHERS' SYSTEMS, INCLUDING ANY DAMAGES.***
### Cli
For a simple cli version of this project. This can be done by running the executable found in dist/
`./dist/main.o`
When exiting the program use 
`quit`
I know that including executables is not standard for c++ projects however idc. 

### Node
While a docker container is advised for running the app. This can also be ran in Node.
`node dist/server.js`
As above in a browser navigate to http://127.0.0.1:3000 

# Library credits:
This project uses several libraries to work as intended. All work on these libraries belongs to the respective owners. Please check out their projects they deserve full credit for the work they put into the javascript and c++ ecosystems respectively:
## Javascript packages:
ws v8.18.0 for websockets found here https://www.npmjs.com/package/ws
express v4.21.2 for server found here https://www.npmjs.com/package/express
typescript v5.7.2 for typescript found here https://www.npmjs.com/package/typescript
## The typescript versions:
@types/ws v8.5.13 found here https://www.npmjs.com/package/@types/ws
@types/express v5.0.0 found here https://www.npmjs.com/package/@types/express

## C++ packages
Catch2 v3.8.0 for testing and debugging found here https://github.com/catchorg/Catch2/tree/devel
Cmake v3.22.1 for compiling tests found here https://cmake.org/

# Special acknowledgements
- My friends for the encouragement while working on this project
- Special thanks is extended to housemates Rob Choi and Ray Syed for the cs specific support with discussing ideas
- My parents for supporting me in all ways but especially the financial support through university. I couldn't have done this project without them.
- The reader for reading this far and trying out this code

