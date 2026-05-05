#include "ant_grid_api.hpp"
#include <iostream>

using namespace ant_grid;

int main() {
    std::cout << "=== Long Test - Food Discovery ===\n";

    auto grid = AntGridAPI::create_grid(3, 3);
    grid->set_random_seed(42);
    grid->set_pheromone_strength(0.0); // Disable pheromones completely

    // Add food at (2,2)
    grid->add_food_source({2, 2}, 5);

    // Deploy ant at (0,0)
    auto ant_id_opt = grid->deploy_ant({0, 0});
    if (!ant_id_opt.has_value()) {
        std::cout << "Failed to deploy ant\n";
        return 1;
    }
    int ant_id = ant_id_opt.value();

    std::cout << "Deployed ant " << ant_id << " at (0,0)\n";
    std::cout << "Food at (2,2)\n";
    std::cout << "Running up to 1000 steps...\n\n";

    // Run up to 1000 steps
    for (int step = 0; step < 1000; ++step) {
        grid->step_simulation();

        auto ant = grid->get_ant(ant_id);
        if (ant) {
            if (ant->food_collected > 0) {
                std::cout << "Step " << step << ": Ant at (" << ant->position.x
                          << ", " << ant->position.y << ") *** FOOD COLLECTED! ***\n";
                std::cout << "Total steps to find food: " << step << "\n";
                return 0;
            }

            // Print every 100 steps
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": Ant at (" << ant->position.x
                          << ", " << ant->position.y << ")\n";
            }
        }
    }

    std::cout << "Food not found after 1000 steps!\n";

    // Check final position
    auto ant = grid->get_ant(ant_id);
    if (ant) {
        std::cout << "Final ant position: (" << ant->position.x << ", " << ant->position.y << ")\n";
    }

    return 0;
}
