#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <stack>
#include <queue>
#include <functional>

template<typename T,
    typename TS1,
    template<typename, typename, typename, typename> class Dictionary, typename TD3, typename TD4,
    template<typename, typename, typename, typename> class Set, typename TS2, typename TS3, typename TS4>
void TrajanAlgorithm(
    const std::set<const T*> &nodes,
    std::function<void(const T*, Set<const TS1*, TS2, TS3, TS4> &)> fixedPointFunc,
    std::function<void(const T*, std::vector<const T*> &)> relationFunc,
    Dictionary<const T*, Set<const TS1*, TS2, TS3, TS4>, TD3, TD4> &results)
{
    std::map<const T*, std::size_t> visitedNodes;
    for (auto node : nodes)
    {
        visitedNodes[node] = 0;
    }
    results.clear();
    std::stack<const T*> visitOrder;
    for (auto node : nodes)
    {
        StrongConnected(
            node,
            fixedPointFunc,
            relationFunc,
            visitedNodes,
            visitOrder,
            results);
    }
}


template<typename T,
    typename TS1,
    template<typename, typename, typename, typename> class Dictionary, typename TD3, typename TD4,
    template<typename, typename, typename, typename> class Set, typename TS2, typename TS3, typename TS4>
void StrongConnected(
    const T* node,
    std::function<void(const T*, Set<const TS1*, TS2, TS3, TS4> &)> fixedPointFunc,
    std::function<void(const T*, std::vector<const T*> &)> relationFunc,
    std::map<const T*, std::size_t> &visitedNodes,
    std::stack<const T*> &visitOrder,
    Dictionary<const T*, Set<const TS1*, TS2, TS3, TS4>, TD3, TD4> &results)
{
    visitOrder.push(node);
    std::size_t stackSize = visitOrder.size();
    visitedNodes[node] = stackSize;
    Set<const TS1*, TS2, TS3, TS4> fixedPoints;
    fixedPointFunc(node, fixedPoints);
    for (auto fixedPoint : fixedPoints)
    {
        results[node].insert(fixedPoint);
    }
    std::vector<const T*> relationPoints;
    relationFunc(node, relationPoints);
    for (auto relationPoint : relationPoints)
    {
        if (visitedNodes[relationPoint] == 0)
        {
            StrongConnected(
                relationPoint,
                fixedPointFunc,
                relationFunc,
                visitedNodes,
                visitOrder,
                results);
        }
        visitedNodes[node] = min(visitedNodes[node], visitedNodes[relationPoint]);
        for (auto connectedNode : results[relationPoint])
        {
            results[node].insert(connectedNode);
        }
    }
    if (visitedNodes[node] == stackSize)
    {
        bool loop;
        do {
            loop = visitOrder.top() != node;
            visitedNodes[visitOrder.top()] = static_cast<std::size_t>(-1);
            for (auto connectedNode : results[node])
            {
                results[visitOrder.top()].insert(connectedNode);
            }
            visitOrder.pop();
        } while (loop);
    }
}
