#include "ant_grid_api.hpp"
#include <iostream>

using namespace ant_grid;

int main() {
    std::cout << "=== Debug Test ===\n";

    auto grid = AntGridAPI::create_grid(5, 5);
    grid->set_random_seed(42);

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

    // Run 20 steps with detailed output
    for (int step = 0; step < 20; ++step) {
        grid->step_simulation();

        auto ant = grid->get_ant(ant_id);
        if (ant) {
            std::cout << "Step " << step << ": Ant at (" << ant->position.x
                      << ", " << ant->position.y << "), State: ";
            switch (ant->state) {
                case AntState::SEARCHING: std::cout << "SEARCHING"; break;
                case AntState::RETURNING_WITH_FOOD: std::cout << "RETURNING"; break;
                case AntState::IDLE: std::cout << "IDLE"; break;
            }
            std::cout << ", Food: " << ant->food_collected << "\n";


            // Check if on food
            bool on_food = grid->is_food_discovered(ant->position);
            std::cout << "  On food: " << (on_food ? "Yes" : "No") << "\n";

            if (ant->food_collected > 0) {
                std::cout << "  *** FOOD COLLECTED! ***\n";
                break;
            }
        }
        std::cout << "\n";
    }

    return 0;
}
