#include "ant_grid_api.hpp"
#include <iostream>

using namespace ant_grid;

int main() {
    std::cout << "=== Ant Grid API Test ===\n\n";

    // Example 1: Basic usage
    std::cout << "1. Basic Usage Example:\n";
    auto grid = AntGridAPI::create_grid(10, 10);

    // Add food sources
    grid->add_food_source({8, 8}, 5);
    grid->add_food_source({1, 8}, 3);
    grid->add_food_source({8, 1}, 4);

    // Deploy ants
    auto ant1_opt = grid->deploy_ant({0, 0});
    auto ant2_opt = grid->deploy_ant({5, 5});
    auto ant3_opt = grid->deploy_ant({9, 9});

    if (!ant1_opt.has_value() || !ant2_opt.has_value() || !ant3_opt.has_value()) {
        std::cout << "Failed to deploy some ants\n";
        return 1;
    }
    int ant1 = ant1_opt.value();
    int ant2 = ant2_opt.value();
    int ant3 = ant3_opt.value();

    std::cout << "Deployed 3 ants with IDs: " << ant1 << ", " << ant2 << ", " << ant3 << "\n";

    // Run simulation with progress tracking
    for (int step = 0; step < 100; ++step) {
        grid->step_simulation();

        if (step % 25 == 0) {
            auto stats = grid->get_statistics();
            std::cout << "Step " << step << ": " << stats.total_food_collected
                      << " food collected, " << stats.food_sources_discovered
                      << " sources discovered\n";
        }

        if (grid->all_food_found()) {
            std::cout << "All food found at step " << step << "!\n";
            break;
        }
    }

    // Example 2: Progress querying
    std::cout << "\n2. Progress Querying Example:\n";
    auto ant_info = grid->get_ant(ant1);
    if (ant_info) {
        std::cout << "Ant " << ant1 << " position: (" << ant_info->position.x
                  << ", " << ant_info->position.y << ")\n";
        std::cout << "Ant " << ant1 << " food collected: " << ant_info->food_collected << "\n";
        std::cout << "Ant " << ant1 << " steps taken: " << ant_info->steps_taken << "\n";
    }

    // Example 3: Multiple ant deployment
    std::cout << "\n3. Multiple Ant Deployment:\n";
    auto grid2 = AntGridAPI::create_grid(15, 15);
    grid2->add_food_source({7, 7}, 20);

    std::vector<Position> start_positions = {{0, 0}, {14, 0}, {0, 14}, {14, 14}, {7, 0}};
    int deployed = grid2->deploy_ants(start_positions);
    std::cout << "Deployed " << deployed << " ants simultaneously\n";

    grid2->run_simulation(500);

    auto final_stats = grid2->get_statistics();
    std::cout << "Final statistics:\n";
    std::cout << "  Total ants: " << final_stats.total_ants_deployed << "\n";
    std::cout << "  Food collected: " << final_stats.total_food_collected << "\n";
    std::cout << "  Average efficiency: " << final_stats.average_efficiency << "\n";

    // Example 4: Configuration options
    std::cout << "\n4. Configuration Example:\n";
    auto grid3 = AntGridAPI::create_grid(12, 12);
    grid3->set_random_seed(123);
    grid3->set_ant_vision_range(3);
    grid3->set_pheromone_strength(2.0);
    grid3->set_pheromone_evaporation_rate(0.02);

    grid3->add_food_source({10, 10}, 8);
    grid3->add_food_source({2, 2}, 6);

    auto ant_id_opt = grid3->deploy_ant({6, 6});
    if (!ant_id_opt.has_value()) {
        std::cout << "Failed to deploy ant\n";
        return 1;
    }
    int ant_id = ant_id_opt.value();

    // Step-by-step progress monitoring
    for (int i = 0; i < 30; ++i) {
        grid3->step_simulation();

        auto ant = grid3->get_ant(ant_id);
        if (ant && i % 5 == 0) {
            std::cout << "Step " << i << ": Ant at (" << ant->position.x
                      << ", " << ant->position.y << "), State: ";
            switch (ant->state) {
                case AntState::SEARCHING: std::cout << "SEARCHING"; break;
                case AntState::RETURNING_WITH_FOOD: std::cout << "RETURNING"; break;
                case AntState::IDLE: std::cout << "IDLE"; break;
            }
            std::cout << "\n";
        }
    }

    std::cout << "\n=== API Demonstration Complete ===\n";

    // Run the built-in demonstrations
    std::cout << "\n=== Running Built-in Demonstrations ===\n";
    AntGridAPI::demonstrate_api_usage();

    return 0;
}
