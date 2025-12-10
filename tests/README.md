# CarRace Unit Tests

Comprehensive unit test suite for the CarRace project using Google Test framework.

## Test Coverage

This test suite contains **285+ unit tests** covering the following modules:

### Game Module (80 tests)
- **Helper Functions** (30 tests): Conversion functions between GLM and PhysX types, quaternion operations
- **Math Utilities** (50 tests): Matrix transformations, vector operations, interpolation, quaternions

### Graphics Module (140 tests)
- **Camera** (40 tests): Construction, pitch/zoom clamping, vector calculations, frustum culling
- **Lights** (50 tests): Light structures, attenuation calculations, spotlight cutoffs, color management
- **Mesh** (50 tests): Vertex/texture structures, normal calculations, tangent/bitangent generation

### Input Module (60 tests)
- **Input Structures** (60 tests): Camera control input, normalization, deadzone, smoothing, state management

### Physics Module (50 tests)
- **Car Control** (50 tests): Steering, throttle, brake, handbrake, velocity, acceleration

### Audio Module (45 tests)
- **Audio Calculations** (45 tests): Volume, pitch, distance attenuation, Doppler effect, stereo panning

## Building the Tests

### Prerequisites
- CMake 3.25 or higher
- C++17 compatible compiler
- Google Test (included as submodule)

### Build Instructions

```powershell
# Navigate to your build directory
cd c:\Users\Grzesiu\OneDrive\Pulpit\sem7\Inzynierka\build

# Configure CMake (pointing to CarRace source directory)
cmake ../CarRace

# Build the test executable
cmake --build . --target carrace_tests --config Debug
```

## Running the Tests

### Run All Tests
```powershell
# From build directory
cd tests\Debug
.\carrace_tests.exe
```

### Run Specific Test Suite
```powershell
# Run only helper function tests
.\carrace_tests.exe --gtest_filter=HelperFunctionsTest.*

# Run only camera tests
.\carrace_tests.exe --gtest_filter=CameraTest.*

# Run only light tests
.\carrace_tests.exe --gtest_filter=*LightTest.*
```

### Verbose Output
```powershell
.\carrace_tests.exe --gtest_verbose
```

### List All Tests
```powershell
.\carrace_tests.exe --gtest_list_tests
```

## Test Organization

```
tests/
├── CMakeLists.txt              # Test build configuration
├── test_utils/
│   └── test_helpers.h          # Custom matchers and utilities
├── game/
│   ├── helper_functions_test.cpp  # 30 tests
│   └── math_utils_test.cpp        # 50 tests
├── gfx/
│   ├── camera_test.cpp            # 40 tests
│   ├── lights_test.cpp            # 50 tests
│   └── mesh_test.cpp              # 50 tests
├── ui/
│   └── input_test.cpp             # 60 tests
├── physics/
│   └── car_control_test.cpp       # 50 tests
└── audio/
    └── audio_test.cpp             # 45 tests
```

## Test Utilities

The `test_utils/test_helpers.h` file provides custom matchers for GLM and PhysX types:

- `EXPECT_VEC3_EQ(expected, actual)` - Compare GLM vec3
- `EXPECT_QUAT_EQ(expected, actual)` - Compare GLM quaternions
- `EXPECT_PXVEC3_EQ(expected, actual)` - Compare PhysX PxVec3
- `EXPECT_PXQUAT_EQ(expected, actual)` - Compare PhysX PxQuat
- `EXPECT_ANGLE_EQ(expected, actual)` - Compare angles with wrapping

## Notes

- Tests are designed to run without requiring OpenGL context or PhysX initialization
- Focus is on pure logic, data structures, and mathematical operations
- Some components (Model, Scene, AudioEngine) require hardware/context and are not fully tested
- All tests use appropriate epsilon values for floating-point comparisons

## Continuous Integration

To disable tests in CI or production builds:

```powershell
# From your build directory
cd c:\Users\Grzesiu\OneDrive\Pulpit\sem7\Inzynierka\build
cmake ../CarRace -DBUILD_TESTS=OFF
```

## Contributing

When adding new tests:
1. Follow the existing naming conventions (`ModuleNameTest` for test fixtures)
2. Group related tests using test fixtures
3. Use descriptive test names that explain what is being tested
4. Add custom matchers to `test_helpers.h` if needed
5. Update this README with test counts
