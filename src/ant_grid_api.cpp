#include "ant_grid_api.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace ant_grid {

AntGrid::AntGrid(int width, int height)
    : width_(width), height_(height),
      pheromone_map_(height, std::vector<double>(width, 0.0)) {
    rng_.seed(std::random_device{}());
}

bool AntGrid::add_food_source(const Position& pos, int amount) {
    if (!is_position_valid(pos) || amount <= 0) {
        return false;
    }
    food_sources_.emplace(pos, FoodSource(pos, amount));
    return true;
}

void AntGrid::clear_food_sources() {
    food_sources_.clear();
}

std::optional<int> AntGrid::deploy_ant(const Position& start_position) {
    if (!is_position_valid(start_position)) {
        return std::nullopt;
    }

    int ant_id = next_ant_id_++;
    ants_.emplace(ant_id, Ant(ant_id, start_position));
    return ant_id;
}

int AntGrid::deploy_ants(const std::vector<Position>& start_positions) {
    int deployed_count = 0;
    for (const auto& pos : start_positions) {
        if (deploy_ant(pos).has_value()) {
            deployed_count++;
        }
    }
    return deployed_count;
}

bool AntGrid::remove_ant(int ant_id) {
    return ants_.erase(ant_id) > 0;
}

void AntGrid::clear_all_ants() {
    ants_.clear();
    next_ant_id_ = 1;
}

void AntGrid::step_simulation() {
    // Update pheromones (evaporation)
    update_pheromones();

    // Move each ant
    for (auto& [id, ant] : ants_) {
        move_ant(ant);
        ant_senses(ant);
        ant.steps_taken++;
    }
}

void AntGrid::run_simulation(int max_steps) {
    int steps = 0;
    while (!all_food_found() && (max_steps == -1 || steps < max_steps)) {
        step_simulation();
        steps++;

        // Print progress every 100 steps
        if (steps % 100 == 0) {
            auto stats = get_statistics();
            std::cout << "Step " << steps << ": "
                      << stats.food_sources_discovered << "/" << food_sources_.size()
                      << " food sources discovered, " << stats.total_food_collected << " units of food collected\n";
        }
    }
}

void AntGrid::reset_simulation() {
    clear_all_ants();
    pheromone_map_ = std::vector<std::vector<double>>(height_, std::vector<double>(width_, 0.0));
    for (auto& [pos, food] : food_sources_) {
        food.discovered = false;
    }
}

GridStats AntGrid::get_statistics() const {
    GridStats stats{};
    stats.total_ants_deployed = ants_.size();

    for (const auto& [id, ant] : ants_) {
        switch (ant.state) {
            case AntState::SEARCHING:
                stats.ants_searching++;
                break;
            case AntState::RETURNING_WITH_FOOD:
                stats.ants_returning++;
                break;
            case AntState::IDLE:
                break;
        }
        stats.total_food_collected += ant.food_collected;
        stats.total_steps_taken += ant.steps_taken;
    }

    stats.food_sources_discovered = 0;
    for (const auto& [pos, food] : food_sources_) {
        if (food.discovered) {
            stats.food_sources_discovered++;
        }
    }

    if (stats.total_steps_taken > 0) {
        stats.average_efficiency = static_cast<double>(stats.total_food_collected) / stats.total_steps_taken;
    } else {
        stats.average_efficiency = 0.0;
    }

    return stats;
}

std::vector<Ant> AntGrid::get_ants() const {
    std::vector<Ant> result;
    result.reserve(ants_.size());
    for (const auto& [id, ant] : ants_) {
        result.push_back(ant);
    }
    return result;
}

std::vector<FoodSource> AntGrid::get_food_sources() const {
    std::vector<FoodSource> result;
    result.reserve(food_sources_.size());
    for (const auto& [pos, food] : food_sources_) {
        result.push_back(food);
    }
    return result;
}

std::optional<Ant> AntGrid::get_ant(int ant_id) const {
    auto it = ants_.find(ant_id);
    if (it != ants_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool AntGrid::is_food_discovered(const Position& pos) const {
    auto it = food_sources_.find(pos);
    return it != food_sources_.end() && it->second.discovered;
}

bool AntGrid::all_food_found() const {
    for (const auto& [pos, food] : food_sources_) {
        if (food.amount > 0) {
            return false;
        }
    }
    return true;
}

void AntGrid::set_random_seed(unsigned int seed) {
    current_seed_ = seed;
    rng_.seed(seed);
}

void AntGrid::set_ant_vision_range(int range) {
    if (range < 1 || range > 10) {
        throw std::invalid_argument("Vision range must be between 1 and 10");
    }
    ant_vision_range_ = range;
}

void AntGrid::set_pheromone_strength(double strength) {
    if (strength < 0.0 || strength > 100.0) {
        throw std::invalid_argument("Pheromone strength must be between 0.0 and 100.0");
    }
    pheromone_strength_ = strength;
}

void AntGrid::set_pheromone_evaporation_rate(double rate) {
    if (rate < 0.0 || rate > 1.0) {
        throw std::invalid_argument("Pheromone evaporation rate must be between 0.0 and 1.0");
    }
    pheromone_evaporation_rate_ = rate;
}

// Configuration getters
unsigned int AntGrid::get_random_seed() const {
    return current_seed_;
}

int AntGrid::get_ant_vision_range() const {
    return ant_vision_range_;
}

double AntGrid::get_pheromone_strength() const {
    return pheromone_strength_;
}

double AntGrid::get_pheromone_evaporation_rate() const {
    return pheromone_evaporation_rate_;
}

// Private methods
Position AntGrid::get_random_move(const Position& current_pos) {
    std::vector<Position> neighbors = get_valid_neighbors(current_pos);
    if (neighbors.empty()) {
        return current_pos;
    }

    std::uniform_int_distribution<int> dist(0, neighbors.size() - 1);
    return neighbors[dist(rng_)];
}

std::vector<Position> AntGrid::get_valid_neighbors(const Position& pos) const {
    std::vector<Position> neighbors;
    const std::vector<Position> directions = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0},  // orthogonal
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}  // diagonal
    };

    for (const auto& dir : directions) {
        Position new_pos = pos + dir;
        if (is_position_valid(new_pos)) {
            neighbors.push_back(new_pos);
        }
    }

    return neighbors;
}

bool AntGrid::is_position_valid(const Position& pos) const {
    return pos.x >= 0 && pos.x < width_ && pos.y >= 0 && pos.y < height_;
}

void AntGrid::update_pheromones() {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            pheromone_map_[y][x] *= (1.0 - pheromone_evaporation_rate_);
        }
    }
}

void AntGrid::move_ant(Ant& ant) {
    Position new_pos = ant.position; // Default to staying put

    if (ant.state == AntState::SEARCHING) {
        // Follow pheromones or explore randomly
        std::vector<Position> neighbors = get_valid_neighbors(ant.position);
        if (!neighbors.empty()) {
            std::vector<double> probabilities;
            double total_pheromone = 0.0;

            for (const auto& neighbor : neighbors) {
                double pheromone = get_pheromone_level(neighbor);
                probabilities.push_back(pheromone);
                total_pheromone += pheromone;
            }

            // If no pheromones, move randomly
            if (total_pheromone < 0.001) {
                new_pos = get_random_move(ant.position);
            } else {
                // Mix of pheromone following and random exploration
                std::uniform_real_distribution<double> exploration_dist(0.0, 1.0);
                double explore_chance = exploration_dist(rng_);

                if (explore_chance < 0.3) { // 30% chance to explore randomly
                    new_pos = get_random_move(ant.position);
                } else {
                    // Weighted random choice based on pheromone levels
                    std::uniform_real_distribution<double> dist(0.0, total_pheromone);
                    double random_value = dist(rng_);
                    double cumulative = 0.0;
                    bool found = false;

                    for (size_t i = 0; i < neighbors.size(); ++i) {
                        cumulative += probabilities[i];
                        if (random_value <= cumulative) {
                            new_pos = neighbors[i];
                            found = true;
                            break;
                        }
                    }

                    // Fallback to random move if weighted selection failed
                    if (!found) {
                        new_pos = get_random_move(ant.position);
                    }
                }
            }
        }
    } else if (ant.state == AntState::RETURNING_WITH_FOOD) {
        // Return home using path memory
        if (!ant.path_history.empty()) {
            new_pos = ant.path_history.back();
            ant.path_history.pop_back();
        } else {
            new_pos = ant.home_position;
        }
    } else {
        return; // IDLE ants don't move
    }

    ant.position = new_pos;
    ant.path_history.push_back(new_pos);
}

void AntGrid::ant_senses(Ant& ant) {
    // Check for food at current position
    auto food_it = food_sources_.find(ant.position);
    if (food_it != food_sources_.end() && food_it->second.amount > 0) {
        if (ant.state == AntState::SEARCHING) {
            ant.state = AntState::RETURNING_WITH_FOOD;
            ant.food_collected++;
            food_it->second.amount--;
            food_it->second.discovered = true;

            // Leave pheromone trail
            deposit_pheromone(ant.position, pheromone_strength_ * 2.0);
        }
    }

    // Check if ant returned home
    if (ant.state == AntState::RETURNING_WITH_FOOD && ant.position == ant.home_position) {
        ant.state = AntState::SEARCHING;
        ant.path_history.clear();
    }

    // Deposit pheromones when searching
    if (ant.state == AntState::SEARCHING) {
        deposit_pheromone(ant.position, pheromone_strength_ * 0.1);
    }
}

void AntGrid::deposit_pheromone(const Position& pos, double amount) {
    if (is_position_valid(pos)) {
        pheromone_map_[pos.y][pos.x] += amount;
        pheromone_map_[pos.y][pos.x] = std::min(pheromone_map_[pos.y][pos.x], 100.0);
    }
}

double AntGrid::get_pheromone_level(const Position& pos) const {
    if (is_position_valid(pos)) {
        return pheromone_map_[pos.y][pos.x];
    }
    return 0.0;
}

// API Factory methods
std::unique_ptr<AntGrid> AntGridAPI::create_grid(int width, int height) {
    return std::make_unique<AntGrid>(width, height);
}

void AntGridAPI::run_standard_test() {
    auto grid = create_grid(20, 20);

    // Add food sources
    grid->add_food_source({15, 15}, 10);
    grid->add_food_source({5, 18}, 8);
    grid->add_food_source({18, 3}, 12);
    grid->add_food_source({2, 2}, 6);

    // Deploy ants from different starting positions
    std::vector<Position> start_positions = {
        {0, 0}, {10, 10}, {19, 0}, {0, 19}, {19, 19}
    };

    int deployed = grid->deploy_ants(start_positions);
    std::cout << "Deployed " << deployed << " ants\n";

    // Run simulation
    grid->run_simulation(1000);

    // Print final statistics
    auto stats = grid->get_statistics();
    std::cout << "\n=== Final Statistics ===\n";
    std::cout << "Total ants deployed: " << stats.total_ants_deployed << "\n";
    std::cout << "Food sources discovered: " << stats.food_sources_discovered << "\n";
    std::cout << "Total food collected: " << stats.total_food_collected << "\n";
    std::cout << "Total steps taken: " << stats.total_steps_taken << "\n";
    std::cout << "Average efficiency: " << std::fixed << std::setprecision(4)
              << stats.average_efficiency << "\n";
}

void AntGridAPI::demonstrate_api_usage() {
    std::cout << "=== Ant Grid API Demonstration ===\n\n";

    // Create grid
    auto grid = create_grid(15, 15);

    // Configure simulation parameters
    grid->set_random_seed(42);
    grid->set_ant_vision_range(2);
    grid->set_pheromone_strength(1.5);
    grid->set_pheromone_evaporation_rate(0.05);

    // Add food sources
    grid->add_food_source({7, 7}, 15);  // Center
    grid->add_food_source({2, 12}, 8);  // Bottom-left
    grid->add_food_source({12, 2}, 10); // Top-right

    // Deploy single ant
    auto ant_id_opt = grid->deploy_ant({0, 0});
    if (!ant_id_opt.has_value()) {
        std::cout << "Failed to deploy ant\n";
        return;
    }
    int ant_id = ant_id_opt.value();
    std::cout << "Deployed ant with ID: " << ant_id << "\n";

    // Run some steps and show progress
    for (int step = 0; step < 50; ++step) {
        grid->step_simulation();

        if (step % 10 == 0) {
            auto ant = grid->get_ant(ant_id);
            if (ant) {
                std::cout << "Step " << step << ": Ant at (" << ant->position.x
                          << ", " << ant->position.y << "), State: ";
                switch (ant->state) {
                    case AntState::SEARCHING: std::cout << "SEARCHING"; break;
                    case AntState::RETURNING_WITH_FOOD: std::cout << "RETURNING"; break;
                    case AntState::IDLE: std::cout << "IDLE"; break;
                }
                std::cout << ", Food collected: " << ant->food_collected << "\n";
            }
        }
    }

    // Deploy more ants
    std::vector<Position> more_positions = {{14, 14}, {0, 14}, {14, 0}};
    int more_deployed = grid->deploy_ants(more_positions);
    std::cout << "\nDeployed " << more_deployed << " additional ants\n";

    // Run to completion with timeout
    grid->run_simulation(5000); // 5000 step limit to prevent infinite loops

    // Final results
    auto final_stats = grid->get_statistics();
    std::cout << "\n=== Simulation Complete ===\n";
    std::cout << "All food found: " << (grid->all_food_found() ? "Yes" : "No") << "\n";
    std::cout << "Total food collected: " << final_stats.total_food_collected << "\n";
    std::cout << "Average efficiency: " << std::fixed << std::setprecision(4)
              << final_stats.average_efficiency << "\n";
}

} // namespace ant_grid
