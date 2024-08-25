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
    std::vector<QPoint> find_path(const QVector<QVector<EPermittedRoadUsers>>& grid, const QPoint& start, const QPoint& goal,
                                  EPermittedRoadUsers user_type);
private:
    double manhattan_heuristic(const QPoint& a, const QPoint& b);
};

#endif // ASTARMAPPATHFINDER_H
