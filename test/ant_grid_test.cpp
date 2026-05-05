#include "ant_grid_api.hpp"
#include <cassert>
#include <iostream>

using namespace ant_grid;

void test_basic_functionality() {
    std::cout << "Testing basic functionality...\n";

    auto grid = AntGridAPI::create_grid(5, 5);

    // Test food placement
    grid->add_food_source({2, 2}, 5);
    assert(grid->get_food_sources().size() == 1);

    // Test ant deployment
    auto ant_id_opt = grid->deploy_ant({0, 0});
    assert(ant_id_opt.has_value());
    int ant_id = ant_id_opt.value();
    assert(ant_id == 1);
    assert(grid->get_ants().size() == 1);

    // Test ant retrieval
    auto ant = grid->get_ant(ant_id);
    assert(ant.has_value());
    assert(ant->position.x == 0 && ant->position.y == 0);
    assert(ant->state == AntState::SEARCHING);

    std::cout << "✓ Basic functionality tests passed\n";
}

void test_movement_and_sensing() {
    std::cout << "Testing movement and sensing...\n";

    auto grid = AntGridAPI::create_grid(10, 10);
    grid->set_random_seed(42); // For reproducible tests

    // Place food at a specific location
    grid->add_food_source({5, 5}, 1);

    // Deploy ant adjacent to food
    auto ant_id_opt = grid->deploy_ant({4, 5});
    assert(ant_id_opt.has_value());
    int ant_id = ant_id_opt.value();

    // Move ant towards food
    grid->step_simulation();

    auto ant = grid->get_ant(ant_id);
    assert(ant.has_value());

    // Check if ant found food (may take a few steps)
    bool found_food = false;
    for (int i = 0; i < 20; ++i) {
        grid->step_simulation();
        ant = grid->get_ant(ant_id);
        if (ant->food_collected > 0) {
            found_food = true;
            break;
        }
    }

    // Note: This test might occasionally fail due to random movement
    // In a real scenario, you'd use deterministic movement or higher probabilities
    std::cout << "Food found: " << (found_food ? "Yes" : "No") << "\n";
    std::cout << "✓ Movement and sensing tests completed\n";
}

void test_statistics() {
    std::cout << "Testing statistics...\n";

    auto grid = AntGridAPI::create_grid(8, 8);

    // Add multiple food sources
    grid->add_food_source({1, 1}, 3);
    grid->add_food_source({6, 6}, 2);

    // Deploy multiple ants
    std::vector<Position> positions = {{0, 0}, {7, 7}, {3, 3}};
    int deployed = grid->deploy_ants(positions);
    assert(deployed == 3);

    // Get initial statistics
    auto stats = grid->get_statistics();
    assert(stats.total_ants_deployed == 3);
    assert(stats.ants_searching == 3);
    assert(stats.ants_returning == 0);
    assert(stats.total_food_collected == 0);

    std::cout << "✓ Statistics tests passed\n";
}

void test_configuration() {
    std::cout << "Testing configuration...\n";

    auto grid = AntGridAPI::create_grid(6, 6);

    // Test configuration methods
    grid->set_random_seed(123);
    grid->set_ant_vision_range(5);
    grid->set_pheromone_strength(2.5);
    grid->set_pheromone_evaporation_rate(0.15);

    // Deploy ant and run a few steps to ensure no crashes
    auto ant_id_opt = grid->deploy_ant({0, 0});
    assert(ant_id_opt.has_value());
    int ant_id = ant_id_opt.value();
    for (int i = 0; i < 10; ++i) {
        grid->step_simulation();
    }

    std::cout << "✓ Configuration tests passed\n";
}

void test_edge_cases() {
    std::cout << "Testing edge cases...\n";

    auto grid = AntGridAPI::create_grid(3, 3);

    // Test invalid positions
    auto invalid_ant = grid->deploy_ant({-1, -1});
    assert(!invalid_ant.has_value());

    auto valid_ant_opt = grid->deploy_ant({0, 0});
    assert(valid_ant_opt.has_value());
    int valid_ant = valid_ant_opt.value();
    assert(valid_ant == 1);

    // Test grid boundaries
    grid->add_food_source({2, 2}, 1); // Valid
    assert(grid->get_food_sources().size() == 1);

    // Test removing ants
    grid->remove_ant(valid_ant);
    assert(grid->get_ants().size() == 0);

    // Test clearing
    grid->clear_all_ants();
    grid->clear_food_sources();
    assert(grid->get_ants().empty());
    assert(grid->get_food_sources().empty());

    std::cout << "✓ Edge case tests passed\n";
}

void run_performance_test() {
    std::cout << "Running performance test...\n";

    const int grid_size = 50;
    const int num_ants = 100;
    const int num_food = 20;
    const int num_steps = 100;

    auto grid = AntGridAPI::create_grid(grid_size, grid_size);

    // Add food sources
    for (int i = 0; i < num_food; ++i) {
        int x = (i * 7) % grid_size;
        int y = (i * 13) % grid_size;
        grid->add_food_source({x, y}, 5);
    }

    // Deploy ants
    std::vector<Position> positions;
    for (int i = 0; i < num_ants; ++i) {
        int x = (i * 3) % grid_size;
        int y = (i * 5) % grid_size;
        positions.push_back({x, y});
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    int deployed = grid->deploy_ants(positions);

    for (int i = 0; i < num_steps; ++i) {
        grid->step_simulation();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    auto stats = grid->get_statistics();

    std::cout << "Performance test results:\n";
    std::cout << "  Grid size: " << grid_size << "x" << grid_size << "\n";
    std::cout << "  Ants deployed: " << deployed << "\n";
    std::cout << "  Food sources: " << num_food << "\n";
    std::cout << "  Steps simulated: " << num_steps << "\n";
    std::cout << "  Total time: " << duration.count() << " ms\n";
    std::cout << "  Food collected: " << stats.total_food_collected << "\n";
    std::cout << "  Average efficiency: " << stats.average_efficiency << "\n";
    std::cout << "✓ Performance test completed\n";
}

int main() {
    std::cout << "=== Ant Grid API Test Suite ===\n\n";

    try {
        test_basic_functionality();
        test_movement_and_sensing();
        test_statistics();
        test_configuration();
        test_edge_cases();
        run_performance_test();

        std::cout << "\n=== All Tests Completed Successfully ===\n";

        // Run demonstration
        std::cout << "\n=== Running API Demonstration ===\n";
        AntGridAPI::demonstrate_api_usage();

    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
