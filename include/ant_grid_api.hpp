#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <memory>
#include <chrono>
#include <optional>

namespace ant_grid {

struct Position {
    int x, y;

    Position() : x(0), y(0) {}
    Position(int x_, int y_) : x(x_), y(y_) {}

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }

    Position operator+(const Position& other) const {
        return Position(x + other.x, y + other.y);
    }

    struct Hash {
        size_t operator()(const Position& pos) const {
            return std::hash<int>{}(pos.x) ^ (std::hash<int>{}(pos.y) << 1);
        }
    };
};

enum class AntState {
    SEARCHING,
    RETURNING_WITH_FOOD,
    IDLE
};

struct Ant {
    int id;
    Position position;
    Position home_position;
    AntState state;
    int steps_taken;
    int food_collected;
    std::vector<Position> path_history;

    Ant(int id_, const Position& start)
        : id(id_), position(start), home_position(start),
          state(AntState::SEARCHING), steps_taken(0), food_collected(0) {}
};

struct FoodSource {
    Position position;
    int amount;
    bool discovered;

    FoodSource(const Position& pos, int amount_)
        : position(pos), amount(amount_), discovered(false) {}
};

struct GridStats {
    int total_ants_deployed;
    int ants_searching;
    int ants_returning;
    int food_sources_discovered;
    int total_food_collected;
    int total_steps_taken;
    double average_efficiency;
};

class AntGrid {
public:
    AntGrid(int width, int height);
    ~AntGrid() = default;

    // Grid configuration
    bool add_food_source(const Position& pos, int amount);
    void clear_food_sources();

    // Ant management
    std::optional<int> deploy_ant(const Position& start_position);
    int deploy_ants(const std::vector<Position>& start_positions);
    bool remove_ant(int ant_id);
    void clear_all_ants();

    // Simulation control
    void step_simulation();
    void run_simulation(int max_steps = -1);
    void reset_simulation();

    // Progress queries
    GridStats get_statistics() const;
    std::vector<Ant> get_ants() const;
    std::vector<FoodSource> get_food_sources() const;
    std::optional<Ant> get_ant(int ant_id) const;
    bool is_food_discovered(const Position& pos) const;
    bool all_food_found() const;

    // Configuration
    void set_random_seed(unsigned int seed);
    void set_ant_vision_range(int range);
    void set_pheromone_strength(double strength);
    void set_pheromone_evaporation_rate(double rate);

    // Configuration getters
    unsigned int get_random_seed() const;
    int get_ant_vision_range() const;
    double get_pheromone_strength() const;
    double get_pheromone_evaporation_rate() const;

private:
    int width_, height_;
    std::vector<std::vector<double>> pheromone_map_;
    std::unordered_map<Position, FoodSource, Position::Hash> food_sources_;
    std::unordered_map<int, Ant> ants_;
    std::mt19937 rng_;
    unsigned int current_seed_ = 42;

    // Configuration
    int ant_vision_range_ = 3;
    double pheromone_strength_ = 1.0;
    double pheromone_evaporation_rate_ = 0.1;

    // Internal methods
    Position get_random_move(const Position& current_pos);
    bool is_position_valid(const Position& pos) const;
    std::vector<Position> get_valid_neighbors(const Position& pos) const;
    void update_pheromones();
    void move_ant(Ant& ant);
    void ant_senses(Ant& ant);
    void deposit_pheromone(const Position& pos, double amount);
    double get_pheromone_level(const Position& pos) const;
    int next_ant_id_ = 1;
};

// API Factory class for easier usage
class AntGridAPI {
public:
    static std::unique_ptr<AntGrid> create_grid(int width, int height);
    static void run_standard_test();
    static void demonstrate_api_usage();
};

} // namespace ant_grid
