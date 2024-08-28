#ifndef ASTARMAPPATHFINDER_H
#define ASTARMAPPATHFINDER_H

#include "MapElements/StationaryMapElements/croadelement.h"
#include <QPoint>
#include <vector>

struct Node {
    /**
     * @brief position on the grid
     */
    QPoint position;
    /**
     * @brief g_cost - cost from start node to this node
     */
    double g_cost;
    /**
     * @brief h_cost - heuristic cost to the goal
     */
    double h_cost;
    /**
     * @brief parent - pointer to the parent node
     */
    Node *parent;

    double total_cost() const {return g_cost + h_cost;}

    bool operator<(const Node& other) const{
        // Higher total cost means lower priority
        return total_cost() > other.total_cost();
    }
};

class AStarMapPathFinder
{
public:
    AStarMapPathFinder();
    /**
     * @brief Finds the path from specified start position to the goal position in the grid, which can be used by the road user
     * of type user_type.
     * @param grid - representation of the map in which path is to be found
     * @param start - starting position (current road user position)
     * @param goal - end position
     * @param user_type - specifies for which type of road user the path should be traversable
     * @return returns path if it was found; otherwise returns empty vector
     */
    std::vector<QPoint> find_path(const QVector<QVector<EPermittedRoadUsers>>& grid, const QPoint& start, const QPoint& goal,
                                  EPermittedRoadUsers user_type);
private:
    double manhattan_heuristic(const QPoint& a, const QPoint& b);
};

#endif // ASTARMAPPATHFINDER_H
