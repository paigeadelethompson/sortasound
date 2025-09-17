# SortaSound - Advanced FM Synthesizer

SortaSound is an advanced FM (Frequency Modulation) synthesizer application built with Qt6 and C++23. It features a comprehensive user interface with multiple synthesizer tabs, a virtual keyboard, tracker functionality, and extensive FM synthesis capabilities.

## Features

- **Advanced FM Synthesis**: 32 different FM algorithms with 6 operators each
- **Multi-tab Interface**: Multiple synthesizer instances with independent controls
- **Virtual Keyboard**: On-screen keyboard with customizable octave ranges
- **Tracker Interface**: Pattern-based music composition
- **Real-time Effects**: Reverb, chorus, and distortion effects
- **Preset Management**: Built-in preset system for quick sound design
- **MIDI Support**: Full MIDI controller integration
- **High-Quality Audio**: 44.1kHz sample rate with 16-bit audio output

## System Requirements

### Minimum Requirements
- **Operating System**: Linux, macOS, or Windows
- **Compiler**: GCC 11+ or Clang 14+ with C++23 support
- **CMake**: Version 3.16 or higher
- **Qt6**: Version 6.0 or higher
- **RAM**: 512 MB minimum
- **Storage**: 50 MB free space

### Required Qt6 Components
- Qt6::Core
- Qt6::Widgets  
- Qt6::Multimedia

## Installation

### Prerequisites

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-multimedia-dev
```

#### Fedora/RHEL/CentOS
```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel qt6-qtmultimedia-devel
```

#### Arch Linux
```bash
sudo pacman -S base-devel cmake qt6-base qt6-multimedia
```

#### macOS (with Homebrew)
```bash
brew install cmake qt6
```

#### Windows
1. Install Visual Studio 2022 or later with C++ development tools
2. Install CMake from [cmake.org](https://cmake.org/download/)
3. Install Qt6 from [qt.io](https://www.qt.io/download-qt-installer)

### Building from Source

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd seroquel
   ```

2. **Create build directory**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake**
   ```bash
   cmake ..
   ```
   
   For Debug builds:
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   ```
   
   For Release builds:
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release ..
   ```

4. **Build the project**
   ```bash
   cmake --build .
   ```

5. **Install (optional)**
   ```bash
   sudo cmake --install .
   ```

### Build Options

You can customize the build with the following CMake options:

```bash
# Set build type
cmake -DCMAKE_BUILD_TYPE=Release ..

# Set installation prefix
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..

# Enable verbose output
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
```

## Running SortaSound

### From Build Directory
```bash
./SortaSound
```

### If Installed System-wide
```bash
SortaSound
```

### Command Line Options
```bash
SortaSound [options]
```

Available options:
- `--help`: Show help message
- `--version`: Display version information

## Usage

### Basic Operation

1. **Launch the application** - The main window will open with the synthesizer interface
2. **Play notes** - Use your computer keyboard or the on-screen virtual keyboard
3. **Adjust parameters** - Use the sliders and controls to modify the sound
4. **Switch algorithms** - Select different FM algorithms from the dropdown menu
5. **Apply effects** - Adjust reverb, chorus, and distortion levels

### Keyboard Mapping

The application supports computer keyboard input for playing notes:
- **A, S, D, F, G, H, J, K**: White keys (C, D, E, F, G, A, B, C)
- **W, E, R, T, Y, U**: Black keys (C#, D#, F#, G#, A#)
- **Z, X**: Octave down/up controls

### Multiple Synthesizer Tabs

- Click the "+" button to add new synthesizer instances
- Each tab operates independently with its own settings
- Close tabs using the "×" button on each tab

### Tracker Interface

- Access the tracker tab for pattern-based composition
- Create musical sequences with precise timing
- Export your compositions

## Development

### Project Structure

```
seroquel/
├── CMakeLists.txt          # Build configuration
├── include/                # Header files
│   ├── fm/                # FM synthesis engine
│   ├── widget/            # UI widgets
│   └── window/            # Main window components
├── src/                   # Source files
│   ├── fm/               # FM synthesis implementation
│   ├── widget/           # Widget implementations
│   └── window/           # Window implementations
└── doc/                  # Documentation
```

### Building for Development

1. **Debug build with symbols**
   ```bash
   mkdir build-debug
   cd build-debug
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   make -j$(nproc)
   ```

2. **Run with debugging**
   ```bash
   gdb ./SortaSound
   ```

### Code Style

The project follows these coding standards:
- C++23 standard
- Qt6 coding conventions
- Consistent indentation (4 spaces)
- Comprehensive documentation comments

## Troubleshooting

### Common Issues

**Qt6 not found**
```bash
# Ensure Qt6 is properly installed
pkg-config --modversion Qt6Core
```

**CMake version too old**
```bash
# Update CMake
sudo apt install cmake  # Ubuntu/Debian
brew upgrade cmake      # macOS
```

**Build errors**
```bash
# Clean and rebuild
rm -rf build
mkdir build && cd build
cmake ..
make clean
make -j$(nproc)
```

**Audio not working**
- Check audio device permissions
- Verify Qt6 Multimedia is properly installed
- Test with other audio applications

### Getting Help

If you encounter issues:
1. Check the troubleshooting section above
2. Verify all dependencies are correctly installed
3. Try a clean build
4. Check system audio settings

## Contributing

We welcome contributions! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Credits

- **Author**: Paige Thompson <paige@paige.bio>
- **Copyright**: 2024 Paige Thompson
- **License**: GPL-3.0

## Acknowledgments

- Built with Qt6 framework
- Uses advanced FM synthesis algorithms
- Inspired by classic FM synthesizers

---

For more information, visit the project documentation in the `doc/` directory.
