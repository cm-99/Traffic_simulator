#include "astarmappathfinder.h"
#include <queue>

AStarMapPathFinder::AStarMapPathFinder() {}

std::vector<QPoint> AStarMapPathFinder::find_path(const QVector<QVector<EPermittedRoadUsers> > &grid, const QPoint &start, const QPoint &goal, EPermittedRoadUsers user_type)
{
    std::priority_queue<Node> open_list;
    QHash<QPoint, Node> all_nodes;

    Node start_node = {start, 0, manhattan_heuristic(start, goal), nullptr};
    open_list.push(start_node);
    all_nodes[start] = start_node;

    std::vector<QPoint> directions = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

    while (!open_list.empty()) {
        Node current_node = open_list.top();
        open_list.pop();

        if (current_node.position == goal) {
            std::vector<QPoint> path;
            Node* path_node = &current_node;
            while (path_node) {
                path.push_back(path_node->position);
                path_node = path_node->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const QPoint& direction : directions) {
            QPoint neighbor_pos = current_node.position + direction;

            if (neighbor_pos.x() < 0 || neighbor_pos.y() < 0 ||
                neighbor_pos.x() >= grid.size() || neighbor_pos.y() >= grid[0].size()) {
                continue;
            }

            if (grid[neighbor_pos.x()][neighbor_pos.y()] != user_type &&
                grid[neighbor_pos.x()][neighbor_pos.y()] != EPermittedRoadUsers::all) {
                continue;
            }

            double tentative_g_cost = current_node.g_cost + 1;

            if (!all_nodes.contains(neighbor_pos) || tentative_g_cost < all_nodes[neighbor_pos].g_cost) {
                Node neighbor_node = { neighbor_pos, tentative_g_cost, manhattan_heuristic(neighbor_pos, goal), &all_nodes[current_node.position] };
                all_nodes[neighbor_pos] = neighbor_node;
                open_list.push(neighbor_node);
            }
        }
    }

    return {};
}

double AStarMapPathFinder::manhattan_heuristic(const QPoint &a, const QPoint &b)
{
    return std::abs(a.x() - b.x()) + std::abs(a.y() - b.y());
}
