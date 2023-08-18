# GUI screenshoter

## Describtion
Qt-based GUI app, that make screenshot on the timer. Check every next screenshot with previous and gives similarity in %.
<br>
Every calculation of similarity and checksum goes in separate thread.
<br>
Also have SQLite-based database that store all of screenshots and retrive last after opening app again.

## Compile and Run
```shell
cmake -S . -B build && cmake --build build
```

```shell
./build/screenshoter
```

## GUI View
<img src="https://github.com/dindin28/Screenshoter/blob/master/resources/pics/example.jpg" alt="Example View" />