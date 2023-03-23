#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <optional>
#include <algorithm>
#include <regex>
#include <queue>

#ifdef DEBUG
#include <cassert>
#endif

// Limits of accepted indices of nodes
constexpr uint32_t LEFT_NUMERIC_LIMIT = 1;
constexpr uint32_t RIGHT_NUMERIC_LIMIT = 999999999;

enum gate_type {
    START,
    NOT,
    XOR,
    AND,
    NAND,
    OR,
    NOR,
    INVALID_TYPE
};


/********************************
 *
 * ERROR HANDLING
 *
********************************/

// Error types
enum err_type {
    SYNTAX_ERROR,
    REPEATED_NODE,
    CYCLE
};

inline void display_error(err_type error, uint64_t line_number = 0,
                          const std::string &line = "", uint32_t node_id = 0)
{
    switch (error) {
        case SYNTAX_ERROR:
            std::cerr << "Error in line " << line_number << ": " << line << "\n";
            break;
        case REPEATED_NODE:
            std::cerr << "Error in line " << line_number << ": signal "
                      << node_id << " is assigned to multiple outputs.\n";
            break;
        case CYCLE:
            std::cerr << "Error: sequential logic analysis has not yet been implemented.\n";
            break;
        default:
#ifdef DEBUG
            assert(false);
#endif
            break;
    }
}


/********************************
 *
 *          DATA TYPES
 *
********************************/


// Gate type + signal's value + incoming edges
using Node = std::tuple<gate_type, bool, std::vector<uint32_t>>;
// Node + its index
using Node_Spec = std::pair<Node, uint32_t>;
// Node's index (i.e. key) + Node's data
using Graph = std::unordered_map<uint32_t, Node>;


/********************************
 *
 *    ACCESS TO A NODE'S DATA
 *
 ********************************/

inline gate_type get_gate_type(const Node &node) {
    return std::get<0>(node);
}

inline bool &get_signal(Node &node) {
    return std::get<1>(node);
}

inline bool cget_signal(const Node &node) {
    return std::get<1>(node);
}

inline std::vector<uint32_t> &get_incoming_edges(Node &node) {
    return std::get<2>(node);
}


/********************************
 *
 *  PARSING AND BUILDING A GRAPH
 *
 ********************************/


gate_type get_type(const std::string &name) {
    if (name == "NOT") {
        return NOT;
    }
    else if (name == "XOR") {
        return XOR;
    }
    else if (name == "AND") {
        return AND;
    }
    else if (name == "NAND") {
        return NAND;
    }
    else if (name == "OR") {
        return OR;
    }
    else if (name == "NOR") {
        return NOR;
    }
    else {
        return INVALID_TYPE;
    }
}

/**
 * If the string represents a number in range @p [LEFT_NUMERIC_LIMIT, @p RIGHT_NUMERIC_LIMIT],
 * returns it as a @p uint32_t. Otherwise, returns an empty object. The function
 * assumes there are no whitespaces in the string.
 */
inline std::optional<uint32_t> get_number(const std::string &s) {
#ifdef DEBUG
    assert(sizeof(unsigned long) >= sizeof(uint32_t));
#endif
    std::size_t characters_processed;
    try {
        const unsigned long number = std::stoul(s, &characters_processed);
        if (number < LEFT_NUMERIC_LIMIT || number > RIGHT_NUMERIC_LIMIT || characters_processed < s.length()) {
            return {};
        }
        return { static_cast<uint32_t>(number) };
    } catch (const std::exception &e) {
        return {};
    }
}

/**
 * Parses the given string extracting information of a single node
 * and returns it wrapped in an @p std::optional object. If the line
 * has some syntax errors, the function returns an empty object.
 */
std::optional<Node_Spec> get_node(const std::string &line) {
    const std::regex reg("(?:^|\\s+)([^\\s]+)");
    std::sregex_iterator rin(line.begin(), line.end(), reg);
    std::sregex_iterator rend;

    if (rin == rend) {
        return {};
    }

    const gate_type type = get_type(rin->str(1));
    ++rin;

    if (type == INVALID_TYPE || rin == rend) {
        return {};
    }

    const std::optional<uint32_t> node_id = get_number(rin->str(1));
    ++rin;

    if (!node_id.has_value() || rin == rend) {
        return {};
    }

    std::vector<uint32_t> incoming_nodes;

    while (rin != rend) {
        const std::optional<uint32_t> out_node = get_number(rin->str(1));
        if (!out_node.has_value()) {
            return {};
        }
        incoming_nodes.push_back(out_node.value());
        ++rin;
    }

    if ((type == NOT && incoming_nodes.size() != 1)
        || (type == XOR && incoming_nodes.size() != 2)
        || (type != NOT && type != XOR && incoming_nodes.size() < 2)) {
        return {};
    }
    else {
        return { { { type, false, incoming_nodes }, node_id.value() } };
    }
}

/**
 * The function adds the missing start nodes to the graph.
 */
inline void add_start_nodes(Graph &graph) {
    for (auto &node : graph) {
        std::vector<uint32_t> &incoming_edges = get_incoming_edges(node.second);
        for (auto edge : incoming_edges) {
            if (graph.find(edge) == graph.end()) {
                graph[edge] = { START, false, {} };
            }
        }
    }
}

/**
 * The function converts the input to a graph. If during building the graph
 * an error pops up, the function continues loading data from the user,
 * but it returns an empty @p std::optional object. Otherwise, it returns
 * the complete graph.
 */
std::optional<Graph> get_graph() {
    Graph graph;
    std::string line;
    uint64_t line_number = 1;
    bool found_error = false;

    while (std::getline(std::cin, line)) {
        std::optional<Node_Spec> node = get_node(line);

        if (not node.has_value()) {
            display_error(SYNTAX_ERROR, line_number, line);
            found_error = true;
        }
        else {
            if (graph.find(node.value().second) != graph.end()) {
                display_error(REPEATED_NODE, line_number, "", node.value().second);
                found_error = true;
            }
            else {
                graph[node.value().second] = node.value().first;
            }
        }

        ++line_number;
    }

    if (not found_error) {
        add_start_nodes(graph);
        return { graph };
    }
    else {
        return {};
    }
}


/********************************
 *
 *      TOPOLOGICAL SORT
 *
 ********************************/


/**
 * The function finds an order of vertices in which the signals
 * go through the graph. The resulting @p std::vector contains
 * all starting nodes in the increasing order. If a cycle exists
 * in the graph, the function terminates returning
 * an empty @p std::optional object.
 */
std::optional<std::vector<uint32_t>> topo_sort(Graph &graph) {
    std::vector<uint32_t> simulation_order;
    std::unordered_map<uint32_t, uint32_t> parents;
    std::queue<uint32_t> queue;

    // Adding the start nodes to the queue
    for (auto &node : graph) {
        if (get_incoming_edges(node.second).empty()) {
            simulation_order.push_back(node.first);
        }
        for (auto neighbour : get_incoming_edges(node.second)) {
            ++parents[neighbour];
        }
    }

    const uint32_t start_nodes = simulation_order.size();
    // Ordering the start nodes
    std::sort(simulation_order.begin(), simulation_order.end());

    for (auto &node : graph) {
        if (parents[node.first] == 0 && get_gate_type(node.second) != START) {
            queue.push(node.first);
        }
    }

    while (not queue.empty()) {
        uint32_t node_ID = queue.front();
        queue.pop();
        simulation_order.push_back(node_ID);

        for (auto neighbour : get_incoming_edges(graph[node_ID])) {
            uint32_t &parent = parents[neighbour];
#ifdef DEBUG
            assert(parent > 0);
#endif
            --parent;
            if (parent == 0 && get_gate_type(graph[neighbour]) != START) {
                queue.push(neighbour);
            }
        }
    }

    // Cycle
    if (simulation_order.size() < graph.size()) {
        return { };
    }

    // Ordering the non-start nodes
    std::reverse(simulation_order.begin() + start_nodes, simulation_order.end());

    return { simulation_order };
}


/********************************
 *
 *          SIMULATION
 *
 ********************************/


inline bool compute_not(Graph &graph, const Graph::iterator &node) {
    return not get_signal(graph.find(get_incoming_edges(node->second)[0])->second);
}

inline bool compute_xor(Graph &graph, const Graph::iterator &node) {
    const std::vector<uint32_t> &incoming_edges = get_incoming_edges(node->second);
    bool sig_1 = get_signal(graph.find(incoming_edges[0])->second);
    bool sig_2 = get_signal(graph.find(incoming_edges[1])->second);
    return sig_1 ^ sig_2;
}

inline bool compute_and(Graph &graph, const Graph::iterator &node) {
    const std::vector<uint32_t> &incoming_edges = get_incoming_edges(node->second);
    for (const auto &parent : incoming_edges) {
        if (not get_signal(graph.find(parent)->second)) {
            return false;
        }
    }

    return true;
}

inline bool compute_nand(Graph &graph, const Graph::iterator &node) {
    return not compute_and(graph, node);
}

inline bool compute_or(Graph &graph, const Graph::iterator &node) {
    const std::vector<uint32_t> &incoming_edges = get_incoming_edges(node->second);
    for (const auto &parent : incoming_edges) {
        if (get_signal(graph.find(parent)->second)) {
            return true;
        }
    }

    return false;
}

inline bool compute_nor(Graph &graph, const Graph::iterator &node) {
    return not compute_or(graph, node);
}

inline bool compute_signal_val(Graph &graph, const Graph::iterator &node) {
#ifdef DEBUG
    assert(node != graph.end());
#endif
    switch (get_gate_type(node->second)) {
        case NOT:
            return compute_not(graph, node);
        case XOR:
            return compute_xor(graph, node);
        case AND:
            return compute_and(graph, node);
        case NAND:
            return compute_nand(graph, node);
        case OR:
            return compute_or(graph, node);
        case NOR:
            return compute_nor(graph, node);
        case START:
            return get_signal(node->second);
        default:
#ifdef DEBUG
            assert(false);
#endif
            return false;
    }
}

inline void print_signals(const Graph &graph, const std::vector<uint32_t> &order) {
    for (auto node_ID : order) {
        if (cget_signal(graph.find(node_ID)->second)) {
            std::cout << '1';
        }
        else {
            std::cout << '0';
        }
    }

    std::cout << '\n';
}

inline bool update_signals(Graph &graph, const std::vector<uint32_t> &node_order,
                           const uint32_t number_of_start_nodes)
{
    for (int32_t i = static_cast<int32_t>(number_of_start_nodes) - 1; i >= 0; --i) {
        auto node = graph.find(node_order[i]);
        get_signal(node->second) = not get_signal(node->second);
        if (get_signal(node->second)) {
            return true;
        }
    }

    return false;
}

/**
 * Returns the number of start nodes in the given graph.
 */
inline uint32_t start_nodes_count(const Graph &graph, const std::vector<uint32_t> &node_order) {
    uint32_t i = 0;
    uint32_t j = node_order.size() - 1;
    uint32_t middle;

    while (i < j) {
        middle = (j - i) / 2 + i;
        if (get_gate_type(graph.find(node_order[middle])->second) != START) {
            j = middle;
        }
        else {
            i = middle + 1;
        }
    }

    return i;
}

/**
 * Performs a simulation going through all permutations of signals
 * that can be assigned to the start nodes and print the resulting
 * state of the system.
 */
void simulation(Graph &graph, const std::vector<uint32_t> &node_order,
                const std::vector<uint32_t> &displaying_order)
{
    const uint32_t number_of_start_nodes = start_nodes_count(graph, node_order);
    do {
        for (uint32_t i = number_of_start_nodes; i < node_order.size(); ++i) {
            get_signal(graph[node_order[i]]) = compute_signal_val(
                graph,
                graph.find(node_order[i])
            );
        }
        print_signals(graph, displaying_order);
    } while (update_signals(graph, node_order, number_of_start_nodes));
}


/********************************
 *
 *            MAIN
 *
 ********************************/


int main() {
    // Parsing the input and building a graph
    std::optional<Graph> graph = get_graph();
    if (not graph.has_value()) {
        return 0;
    }

    // Ordering the nodes
    auto node_order = topo_sort(graph.value());
    
    if (not node_order.has_value()) {
        display_error(CYCLE);
    }
    else {
        // The order in which the nodes should be displayed
        std::vector<uint32_t> displaying_order(graph.value().size());
        uint32_t ID = 0;
        for (const auto &node : graph.value()) {
            displaying_order[ID] = node.first;
            ++ID;
        }

        std::sort(displaying_order.begin(), displaying_order.end());

        // Performing the simulation
        simulation(graph.value(), node_order.value(), displaying_order);
    }

    return 0;
}
