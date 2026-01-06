/**
 * @file SpatialGrid.hpp
 * @brief Spatial partitioning grid for efficient entity queries.
 *
 * @details Provides O(1) insertion/removal and efficient radius-based queries
 * for large maps with many entities. Uses a hash-based spatial grid.
 *
 * @ingroup ECS
 */

#ifndef SPATIAL_GRID_HPP
#define SPATIAL_GRID_HPP

#include <Graphitech/ECS/Core/Types.hpp>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

namespace ECS {

/**
 * @brief Spatial hash grid for efficient proximity queries.
 *
 * @details Partitions 3D space into cells and allows fast lookup of entities
 * within a given radius. Suitable for collision detection, AI queries, and
 * rendering culling in large open worlds.
 */
class SpatialGrid {
private:
    float cellSize_; /**< Size of each grid cell */
    std::unordered_map<uint64_t, std::vector<Entity>> grid_; /**< Cell hash to entities */
    std::unordered_map<Entity, glm::vec3> entityPositions_; /**< Entity position cache */

    /**
     * @brief Computes a hash for a 3D cell position.
     *
     * @param pos World position.
     * @return 64-bit hash uniquely identifying the cell.
     */
    uint64_t hashCell(const glm::vec3& pos) const {
        int cellX = static_cast<int>(pos.x / cellSize_);
        int cellY = static_cast<int>(pos.y / cellSize_);
        int cellZ = static_cast<int>(pos.z / cellSize_);
        return ((uint64_t)cellX << 40) | ((uint64_t)cellY << 20) | (uint64_t)cellZ;
    }

public:
    /**
     * @brief Constructs a spatial grid with specified cell size.
     *
     * @param cellSize Size of each grid cell (default: 10.0f).
     */
    explicit SpatialGrid(float cellSize = 10.0f) : cellSize_(cellSize) {}

    /**
     * @brief Updates or inserts an entity at a given position.
     *
     * @param e The entity to update.
     * @param pos The entity's world position.
     */
    void updateEntity(Entity e, const glm::vec3& pos) {
        auto it = entityPositions_.find(e);
        if (it != entityPositions_.end()) {
            uint64_t oldCell = hashCell(it->second);
            uint64_t newCell = hashCell(pos);
            if (oldCell != newCell) {
                auto& oldVec = grid_[oldCell];
                oldVec.erase(std::remove(oldVec.begin(), oldVec.end(), e), oldVec.end());
                grid_[newCell].push_back(e);
            }
        } else {
            uint64_t cell = hashCell(pos);
            grid_[cell].push_back(e);
        }
        entityPositions_[e] = pos;
    }

    /**
     * @brief Queries entities within a radius of a position.
     *
     * @param pos Center position for the query.
     * @param radius Search radius.
     * @return Vector of entities within the radius.
     */
    std::vector<Entity> getEntitiesNear(const glm::vec3& pos, float radius) const {
        std::vector<Entity> result;
        int cellRange = static_cast<int>(radius / cellSize_) + 1;
        int cx = static_cast<int>(pos.x / cellSize_);
        int cy = static_cast<int>(pos.y / cellSize_);
        int cz = static_cast<int>(pos.z / cellSize_);

        for (int x = cx - cellRange; x <= cx + cellRange; ++x) {
            for (int y = cy - cellRange; y <= cy + cellRange; ++y) {
                for (int z = cz - cellRange; z <= cz + cellRange; ++z) {
                    uint64_t cell = ((uint64_t)x << 40) | ((uint64_t)y << 20) | (uint64_t)z;
                    auto it = grid_.find(cell);
                    if (it != grid_.end()) {
                        for (auto e : it->second) {
                            float dist = glm::distance(entityPositions_.at(e), pos);
                            if (dist <= radius) result.push_back(e);
                        }
                    }
                }
            }
        }
        return result;
    }

    /**
     * @brief Removes an entity from the grid.
     *
     * @param e The entity to remove.
     */
    void removeEntity(Entity e) {
        auto it = entityPositions_.find(e);
        if (it != entityPositions_.end()) {
            uint64_t cell = hashCell(it->second);
            auto& vec = grid_[cell];
            vec.erase(std::remove(vec.begin(), vec.end(), e), vec.end());
            entityPositions_.erase(it);
        }
    }

    /**
     * @brief Clears all entities from the grid.
     */
    void clear() {
        grid_.clear();
        entityPositions_.clear();
    }

    /**
     * @brief Gets the number of entities in the grid.
     *
     * @return Total entity count.
     */
    size_t size() const {
        return entityPositions_.size();
    }
};

} // namespace ECS

#endif // SPATIAL_GRID_HPP
