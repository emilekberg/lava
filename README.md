# lava

## building

```bash
cmake -S . -B ./build
cmake --build ./build --config release
```

## running tests

```bash
ctest --test-dir build
```

## running example game

find the approrpiate project in the build folder and run it. for instance: 

```bash
cd build/playground/pong/Release/
./pong.exe
```

## references

https://vulkan-tutorial.com/

https://github.com/KhronosGroup/Vulkan-Hpp/blob/main/vk_raii_ProgrammingGuide.md