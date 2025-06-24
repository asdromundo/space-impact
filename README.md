
# SDL3 with RmlUi Template
This project is an example for setting up and using SDL3 (and its associated libraries: SDL_Mixer and SDL_Image) alongside [RmlUi](https://mikke89.github.io/RmlUi/) for UI rendering. It uses C++, CMake, and includes support for platforms like macOS, Windows, Linux, iOS, and more. The example demonstrates initializing these libraries and integrating RmlUi into an SDL-based application.

This is based on the [SDL3 App From Source Minimal Example](https://github.com/Ravbug/sdl3-sample) project, with modifications to add RmlUi and remove SDL_TTF support.

See [src/main.cpp](src/main.cpp) for the core example code.

---

### Building And Running
If you're a beginner, consider reading [this](https://github.com/Ravbug/sdl3-sample/wiki/Setting-up-your-computer) for setting up your development environment. Otherwise, ensure you have CMake installed and a compatible compiler, then run the following commands:

```sh
# Clone the repository with submodules to ensure dependencies are downloaded.
git clone https://github.com/your-username/sdl3-rmlui-template --depth=1 --recurse-submodules  --shallow-submodules
cd sdl3-rmlui-template
cmake -S . -B build
```

You can also use the initialization scripts inside [`config/`](config/). Open the generated project in your IDE from the `build/` folder (if configured) and run the application!

---

### Supported Platforms
This project supports a range of platforms. Below is the list of platforms:

| Platform  | Architecture    | Generator         |
|-----------|-----------------|-------------------|
| Windows   | x86_64, arm64   | Visual Studio, MingW    |
| Linux     | x86_64, arm64   | Ninja, Make      |
| Web*      | wasm            | Ninja, Make      |
| Android*  | x86, x64, arm, arm64 | Ninja via Android Studio |

*Further instructions for these platforms are available in [`config/`](config/).

> Note: UWP support was [removed from SDL3](https://github.com/libsdl-org/SDL/pull/10731) during development. For historical reasons, a working UWP sample can be accessed via this commit: [df270da](https://github.com/Ravbug/sdl3-sample/tree/df270daa8d6d48426e128e50c73357dfdf89afbf).

---

### Updating SDL and Dependencies
To update SDL and other dependencies like RmlUi, simply update the respective submodules:

```sh
cd SDL
git pull
cd ..

cd SDL_image
git pull
cd ..

cd SDL_mixer
git pull
cd ..

cd RmlUi
git pull
```

Note: You can use submodules for convenience, as this template does, or opt to copy the source directories directly into your project structure.

---

### Features and Usage
This template includes:
1. **SDL Rendering with RmlUi**: RmlUi provides a modern HTML/CSS-inspired UI system that integrates seamlessly with SDL for rendering UI components.
2. **Dependency Management**: Easily update and manage dependencies via submodules.
3. **Cross-Platform Support**: Build and test across numerous platforms out of the box.
4. **Simple Integration**: Example code shows how to initialize RmlUi with SDL, load UI documents, and integrate them into your render loop.

---

### Reporting Issues
If you run into issues or have suggestions for improvements, feel free to create an Issue or submit a Pull Request to this repository!

### Reference
This project was inspired by the excellent [SDL3 App From Source Minimal Example](https://github.com/Ravbug/sdl3-sample) project by [Ravbug](https://github.com/Ravbug).
