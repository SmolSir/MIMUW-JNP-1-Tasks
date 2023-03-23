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

template<class Virus>
class VirusGenealogy {
private:
    class Node;

    using viral_map_type = std::map<typename Virus::id_type, std::shared_ptr<Node>>;
    using viral_map_it_type = typename std::map<typename Virus::id_type, std::shared_ptr<Node>>::iterator;
    using viral_set_type = std::set<typename Virus::id_type>;
    using viral_set_it_type = typename std::set<typename Virus::id_type>::iterator;
    using map_insert_return_type = std::pair<viral_map_it_type, bool>;
    using set_insert_return_type = std::pair<viral_set_it_type, bool>;

    viral_map_type viral_map;
    std::shared_ptr<Node> stem_node;

    class Node {
    public:
        Virus virus;
        viral_map_type children;
        viral_set_type parents;

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

    void add_erased(std::vector<viral_map_it_type>& to_be_erased,
                    std::map<std::shared_ptr<Node>, viral_map_it_type>& erased_parents,
                    std::multimap<std::shared_ptr<Node>, viral_set_it_type>& erased_children,
                    typename Virus::id_type const &id) {

        auto children_to_erase = viral_map.find(id)->second->children;
        auto parents_to_erase = viral_map.find(id)->second->parents;
        to_be_erased.push_back(viral_map.find(id));

        for (auto child : children_to_erase) {
            auto child_pointer = child.second;
            auto parent_at_child = child_pointer->parents.find(id);
            erased_children.insert({child_pointer, parent_at_child});
            if (erased_children.count(child_pointer) == child_pointer->parents.size()) {
                add_erased(to_be_erased, erased_parents, erased_children, child_pointer->virus.get_id());
            }
        }

        // Żaden ojciec inny niż ojciec pierwszego usuwanego wierzchołka nie straci dziecka
        for (auto parent : parents_to_erase) {
            auto parent_pointer = viral_map.find(parent)->second;
            erased_parents.insert({parent_pointer, parent_pointer->children.find(id)});
        }
    };

public:
    VirusGenealogy<Virus>& operator=(const VirusGenealogy<Virus>&) = delete;
    VirusGenealogy<Virus>& operator=(VirusGenealogy<Virus>&&) = delete;
    VirusGenealogy<Virus>(const VirusGenealogy<Virus>&) = delete;

    class children_iterator {
    private:
        viral_map_it_type iter;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = const Virus;
        using pointer = const Virus*;
        using reference = const Virus&;
        using iterator_category = std::bidirectional_iterator_tag;

        children_iterator() : iter() { };
        children_iterator(viral_map_it_type const &iter) : iter(iter) { };

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
        stem_node = std::make_shared<Node>(stem_id);
        viral_map.insert({stem_id, std::shared_ptr(stem_node)});
    };

    typename Virus::id_type get_stem_id() const {
        return stem_node->virus.get_id();
    };

    VirusGenealogy<Virus>::children_iterator get_children_begin(typename Virus::id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        return children_iterator(viral_map.at(id)->children.begin());
    };

    VirusGenealogy<Virus>::children_iterator get_children_end(typename Virus::id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        return children_iterator(viral_map.at(id)->children.end());
    };

    std::vector<typename Virus::id_type> get_parents(typename Virus::id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        return viral_map.at(id)->parents_vector();
    };

    bool exists(typename Virus::id_type const &id) const {
        return viral_map.contains(id);
    };

    const Virus& operator[](typename Virus::id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFound();
        }
        return viral_map.at(id)->virus;
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

        try {
            if (exists(id)) {
                throw VirusAlreadyCreated();
            }
            for (auto parent : parent_ids) {
                if (!exists(parent)) {
                    throw VirusNotFound();
                }
            }

            if (parent_ids.empty()) {
                return;
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
        std::vector<viral_map_it_type> to_be_erased;
        std::multimap<std::shared_ptr<Node>, viral_set_it_type> erased_children;
        std::map<std::shared_ptr<Node>, viral_map_it_type> erased_parents;

        if (!exists(id)) {
            throw VirusNotFound();
        }
        if (id == get_stem_id()) {
            throw TriedToRemoveStemVirus();
        }

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
    };
};

#endif //VIRUS_VIRUS_GENEALOGY_H
