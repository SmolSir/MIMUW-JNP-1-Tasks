#ifndef VIRUS_VIRUS_GENEALOGY_H
#define VIRUS_VIRUS_GENEALOGY_H

#include <vector>
#include <memory>
#include <map>
#include <set>
#include <iterator>

class VirusNotFound : public std::exception {
    const char* what() const noexcept override {
        return "VirusNotFound";
    }
};

class VirusAlreadyCreated : public std::exception {
    const char* what() const noexcept override {
        return "VirusAlreadyCreated";
    }
};

class TriedToRemoveStemVirus : public std::exception {
    const char* what() const noexcept override {
        return "TriedToRemoveStemVirus";
    }
};

//TODO pointer to implementation
template<class Virus>
class VirusGenealogy {
private:
    class Node {
    public:
        Virus virus;
        std::map<typename Virus::id_type, std::shared_ptr<Node>> children;
        std::set<typename Virus::id_type> parents;

        Node(const typename Virus::id_type &virus_id) : virus(virus_id) { };
        Node() = default;

        std::vector<typename Virus::id_type> parents_vector() const {
            return std::vector(parents.begin(), parents.end());
        };

        auto add_parent(const typename Virus::id_type &parent_id) {
            return parents.insert(parent_id);
        };

        auto add_child(std::shared_ptr<Node>& child_virus) {
            std::shared_ptr<Node> pointer_to_child(child_virus);
            return children.insert({child_virus->virus.get_id(), pointer_to_child});
        };

        bool child_exists(const typename Virus::id_type &child_id) {
            return children.contains(child_id);
        };
    };

    using map_insert_return_type = std::pair<typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator, bool>;
    using set_insert_return_type = std::pair<typename std::set<typename Virus::id_type>::iterator, bool>;

    std::map<typename Virus::id_type, std::shared_ptr<Node>> viral_map;
    std::shared_ptr<Node> stem_node;

    void add_erased(std::vector<typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator>& to_be_erased,
                    std::map<std::shared_ptr<Node>, typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator>& erased_parents,
                    std::multimap<std::shared_ptr<Node>, typename std::set<typename Virus::id_type>::iterator>& erased_children,
                    typename Virus::id_type const &id) {
        auto to_erase = viral_map.find(id);
        to_be_erased.push_back(to_erase);

        for (auto child = to_erase->second->children.begin(); child != to_erase->second->children.end(); ++child) {
            auto child_pointer = child->second;
            auto parent_at_child = child_pointer->parents.find(id);
            erased_children.insert({child_pointer, parent_at_child});
            if (erased_children.count(child_pointer) == child_pointer->parents.size()) {
                add_erased(to_be_erased, erased_parents, erased_children, child_pointer->virus.get_id());
            }
        }

        // Żaden ojciec inny niż ojciec pierwszego usuwanego wierzchołka nie straci dziecka
        for (auto parent = to_erase->second->parents.begin(); parent != to_erase->second->parents.end(); ++parent) {
            auto parent_pointer = viral_map.find(*parent)->second;
            erased_parents.insert({parent_pointer, parent_pointer->children.find(id)});
        }
    };

public:
    //TODO, chyba tak wyglądają te blokady z treści, ale trzeba sprawdzić czy działa
    VirusGenealogy<Virus>& operator=(const VirusGenealogy<Virus>&) = delete;
    VirusGenealogy<Virus>(const VirusGenealogy<Virus>&) = delete;

    class children_iterator {
    public:
        typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator iter;
        children_iterator() : iter() { };
        children_iterator(typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator const &iter) : iter(iter) { };

        using difference_type = std::ptrdiff_t;
        using value_type = const Virus;
        using pointer = const Virus*;
        using reference = const Virus&;
        using iterator_category = std::bidirectional_iterator_tag;

        const Virus& operator*() const {
            return iter->second->virus;
        };

        children_iterator& operator++() {
            ++iter;
            return *this;
        };

        children_iterator operator++(int) {
            auto copy = iter;
            ++iter;
            return copy;
        };

        children_iterator& operator--() {
            --iter;
            return *this;
        };

        children_iterator operator--(int) {
            auto copy = iter;
            --iter;
            return copy;
        };

        bool operator==(const children_iterator &ch_iter) const {
            return this->iter == ch_iter.iter;
        };

        bool operator!=(const children_iterator &ch_iter) const {
            return this->iter != ch_iter.iter;
        };

        const Virus* operator->() const {
            return &(iter->second->virus);
        };
    };

    VirusGenealogy(typename Virus::id_type const &stem_id) {
        try {
            stem_node = std::make_shared<Node>(stem_id);
            viral_map.insert({stem_id, std::shared_ptr(stem_node)});
        }
        catch (...) {
            throw;
        }
    };

    typename Virus::id_type get_stem_id() const {
        try {
            return stem_node->virus.get_id();
        }
        catch (...) {
            throw;
        }
    };

    VirusGenealogy<Virus>::children_iterator get_children_begin(typename Virus::id_type const &id) const {
        try {
            if (!exists(id)) {
                throw VirusNotFound();
            }
            return children_iterator(viral_map.at(id)->children.begin());
        }
        catch (...) {
            throw;
        }
    };

    VirusGenealogy<Virus>::children_iterator get_children_end(typename Virus::id_type const &id) const {
        try {
            if (!exists(id)) {
                throw VirusNotFound();
            }
            return children_iterator(viral_map.at(id)->children.end());
        }
        catch (...) {
            throw;
        }
    };

    std::vector<typename Virus::id_type> get_parents(typename Virus::id_type const &id) const {
        try {
            if (!exists(id)) {
                throw VirusNotFound();
            }
            return viral_map.at(id)->parents_vector();
        }
        catch (...) {
            throw;
        }
    };

    bool exists(typename Virus::id_type const &id) const {
        try {
            return viral_map.contains(id);
        }
        catch (...) {
            throw;
        }
    };

    const Virus& operator[](typename Virus::id_type const &id) const {
        try {
            if (!exists(id)) {
                throw VirusNotFound();
            }
            return viral_map.at(id)->virus;
        }
        catch (...) {
            throw;
        }
    };

    void create(typename Virus::id_type const &id, typename Virus::id_type const &parent_id) {
        map_insert_return_type map_insert_result;
        map_insert_return_type child_insert_result;
        std::shared_ptr<Node> new_virus;
        std::shared_ptr<Node> parent_virus;

        map_insert_result.second = false;
        child_insert_result.second = false;

        try {
            if (exists(id)) {
                throw VirusAlreadyCreated();
            }
            if (!exists(parent_id)) {
                throw VirusNotFound();
            }

            new_virus = std::make_shared<Node>(id);
            parent_virus = viral_map[parent_id];

            map_insert_result = viral_map.insert({id, new_virus});
            child_insert_result = (parent_virus->add_child(new_virus));
            new_virus->add_parent(parent_id);
        }
        catch (...) {
            if (map_insert_result.second) {
                viral_map.erase(map_insert_result.first);
            }
            if (child_insert_result.second) {
                parent_virus->children.erase(child_insert_result.first);
            }
            throw;
        }
    };

    void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids) {
        map_insert_return_type map_insert_result;
        std::shared_ptr<Node> new_virus;
        std::shared_ptr<Node> parent_virus;

        map_insert_result.second = false;

        std::vector<std::pair<std::shared_ptr<Node>, map_insert_return_type>> parents_insert_registry;
        parents_insert_registry.reserve(parent_ids.size());

        if (parent_ids.empty()) {
            return;
        }

        try {
            if (exists(id)) {
                throw VirusAlreadyCreated();
            }
            for (auto parent : parent_ids) {
                if (!exists(parent)) {
                    throw VirusNotFound();
                }
            }

            new_virus = std::make_shared<Node>(id);
            map_insert_result = viral_map.insert({id, new_virus});

            for (auto pid : parent_ids) {
                parent_virus = viral_map[pid];
                if (parent_virus->child_exists(id)) {
                    continue;
                }

                new_virus->add_parent(pid);
                parents_insert_registry.push_back({parent_virus, parent_virus->add_child(new_virus)});
            }
        }
        catch (...) {
            if (map_insert_result.second) {
                viral_map.erase(map_insert_result.first);
            }
            for (auto log : parents_insert_registry) {
                if (log.second.second) {
                    log.first->children.erase(log.second.first);
                }
            }
            throw;
        }
    };

    void connect(typename Virus::id_type const &child_id, typename Virus::id_type const &parent_id) {

        set_insert_return_type child_add_new_parent;
        map_insert_return_type parent_add_new_child;
        std::shared_ptr<Node> child_virus;
        std::shared_ptr<Node> parent_virus;

        child_add_new_parent.second = false;
        parent_add_new_child.second = false;

        try {

            if (!exists(child_id) || !exists(parent_id)) {
                throw VirusNotFound();
            }

            child_virus = viral_map[child_id];
            parent_virus = viral_map[parent_id];

            if (parent_virus->child_exists(child_id)) {
                return;
            }

            child_add_new_parent = child_virus->add_parent(parent_id);
            parent_add_new_child = parent_virus->add_child(child_virus);
        }
        catch (...) {
            if (child_add_new_parent.second) {
                child_virus->parents.erase(child_add_new_parent.first);
            }
            if (parent_add_new_child.second) {
                parent_virus->children.erase(parent_add_new_child.first);
            }
            throw;
        }
    };

    void remove(typename Virus::id_type const &id) {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        if (id == get_stem_id()) {
            throw TriedToRemoveStemVirus();
        }

        try {
            std::vector<typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator> to_be_erased;
            std::multimap<std::shared_ptr<Node>, typename std::set<typename Virus::id_type>::iterator> erased_children;
            std::map<std::shared_ptr<Node>, typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator> erased_parents;
            add_erased(to_be_erased, erased_parents, erased_children, id);

            for (auto child : erased_children) {
                child.first->parents.erase(child.second);
            }

            for (auto parent : erased_parents) {
                parent.first->children.erase(parent.second);
            }

            for (auto erase_from_map : to_be_erased) {
                viral_map.erase(erase_from_map);
            }
            return;
        }
        catch (...) {
            //TODO
            throw;
        }
    };
};

#endif //VIRUS_VIRUS_GENEALOGY_H
