# Ant Grid API

A C++ test API for simulating ant colony optimization on an m x n grid. Ants search for and collect food sources while leaving pheromone trails to guide other ants.

## Features

- **Grid-based simulation**: Configurable m x n grid
- **Ant deployment**: Single or multiple ants from various starting positions
- **Food placement**: Multiple food sources with configurable amounts
- **Pheromone system**: Ants leave chemical trails to guide others
- **Progress tracking**: Real-time statistics and individual ant monitoring
- **Configurable parameters**: Vision range, pheromone strength, evaporation rate

## Core Classes

### `Position`
Represents a grid coordinate (x, y) with hash support for unordered containers.

### `Ant`
- `id`: Unique identifier
- `position`: Current location
- `home_position`: Starting location
- `state`: SEARCHING, RETURNING_WITH_FOOD, or IDLE
- `steps_taken`: Total movement count
- `food_collected`: Amount of food gathered
- `path_history`: Movement trail for homing

### `FoodSource`
- `position`: Grid location
- `amount`: Remaining food units
- `discovered`: Whether ants have found it

### `GridStats`
Comprehensive statistics including ant counts, food discovery, and efficiency metrics.

### `AntGrid`
Main simulation class with methods for:
- Grid configuration and food placement
- Ant deployment and management
- Simulation control (step, run, reset)
- Progress queries and statistics

## API Usage

### Basic Setup
```cpp
#include "ant_grid_api.hpp"
using namespace ant_grid;

// Create grid
auto grid = AntGridAPI::create_grid(width, height);

// Add food sources
bool success = grid->add_food_source({x, y}, amount);

// Deploy ants
auto ant_id = grid->deploy_ant({start_x, start_y});
if (ant_id.has_value()) {
    // Ant successfully deployed
}
```

### Progress Monitoring
```cpp
// Get overall statistics
auto stats = grid->get_statistics();
std::cout << "Food collected: " << stats.total_food_collected << "\n";

// Query individual ant
auto ant = grid->get_ant(ant_id);
if (ant) {
    std::cout << "Ant position: (" << ant->position.x << ", " << ant->position.y << ")\n";
    std::cout << "State: " << (int)ant->state << "\n";
}

// Check food discovery
bool found = grid->is_food_discovered({x, y});
bool all_found = grid->all_food_found();
```

### Simulation Control
```cpp
// Single step
grid->step_simulation();

// Run to completion or max steps
grid->run_simulation(1000);

// Reset for new simulation
grid->reset_simulation();
```

### Configuration
```cpp
grid->set_random_seed(42);
grid->set_ant_vision_range(3);
grid->set_pheromone_strength(1.5);
grid->set_pheromone_evaporation_rate(0.1);
```

## Key Methods

### Grid Management
- `add_food_source(pos, amount)`: Place food at location, returns success status
- `clear_food_sources()`: Remove all food

### Ant Management
- `deploy_ant(start_pos)`: Deploy single ant, returns optional ID
- `deploy_ants(positions)`: Deploy multiple ants, returns count
- `remove_ant(id)`: Remove specific ant, returns success status
- `clear_all_ants()`: Remove all ants

### Progress Queries
- `get_statistics()`: Comprehensive stats
- `get_ants()`: All ant data
- `get_food_sources()`: All food source data
- `get_ant(id)`: Specific ant data
- `is_food_discovered(pos)`: Check if food found
- `all_food_found()`: Check completion

## Algorithm Details

### Ant Behavior
1. **Searching**: Random exploration biased by pheromone trails
2. **Food Detection**: Immediate pickup when on food source
3. **Returning**: Direct path home using memory
4. **Pheromone Deposition**: Stronger when returning with food

### Pheromone System
- Ants deposit pheromones at current position
- Pheromones evaporate over time (configurable rate)
- Searching ants follow pheromone gradients
- Weighted random selection based on pheromone levels

### Movement Logic
- 8-directional movement (orthogonal + diagonal)
- Boundary checking prevents leaving grid
- Path memory for homing behavior
- Vision range for local sensing

## Project Structure

```
ants/
├── include/                # Public headers
│   └── ant_grid_api.hpp    # Public API header
├── src/                    # Implementation source code
│   └── ant_grid_api.cpp    # Library implementation
├── test/                   # Test code and examples
│   ├── ant_grid_test.cpp   # Main test suite
│   ├── example_usage.cpp   # Usage examples
│   └── *.cpp               # Additional test files
├── CMakeLists.txt          # Build configuration
└── README.md               # This documentation
```

## Building

The API uses CMake for building and only standard C++17 features:

```bash
mkdir build && cd build
cmake ..
make
```

## Testing

Run comprehensive tests:
```bash
make run_tests
```

Run example demonstration:
```bash
make run_example
```

Or run executables directly:
```bash
./ant_grid_test
./ant_grid_example
```

## Performance

Designed for efficiency with:
- O(1) ant lookup using unordered_map
- Grid-based pheromone storage
- Minimal memory allocation during simulation
- Configurable parameters for tuning

## Example Output

```
=== Ant Grid API Demonstration ===

Deployed ant with ID: 1
Step 0: Ant at (0, 0), State: SEARCHING, Food collected: 0
Step 10: Ant at (3, 2), State: SEARCHING, Food collected: 0
Step 20: Ant at (5, 4), State: RETURNING, Food collected: 1
Step 30: Ant at (2, 1), State: SEARCHING, Food collected: 1

=== Simulation Complete ===
All food found: Yes
Total food collected: 3
Average efficiency: 0.0147
```

## Extensions

The API is designed for extensibility:
- Add new ant behaviors
- Implement different pheromone types
- Add obstacles or terrain
- Support multiple ant colonies
- Visual rendering capabilities
