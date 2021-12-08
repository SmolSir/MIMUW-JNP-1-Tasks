#ifndef VIRUS_VIRUS_GENEALOGY_H
#define VIRUS_VIRUS_GENEALOGY_H

#include <vector>
#include <memory>
#include <map>

class VirusNotFoundException : public std::exception {
    virtual const char* what() const throw() {
        return "VirusNotFound";
    }
};

class VirusAlreadyCreatedException : public std::exception {
    virtual const char* what() const throw() {
        return "VirusAlreadyCreated";
    }
};

class TriedToRemoveStemVirusException : public std::exception {
    virtual const char* what() const throw() {
        return "TriedToRemoveStemVirus";
    }
};

template<typename Virus>
class VirusGenealogy {
private:
    class Node {
    public:
        Virus virus;
        const Virus::id_type virus_id;
        std::vector<std::shared_ptr<Node>> children;
        std::vector<typename Virus::id_type> parents;

        Node(Virus::id_type const &virus_id) : virus(virus_id), virus_id(virus_id) {}
        ~Node() {
          viral_map.erase(virus_id); // wyjątek?
        }

        void add_parent(Virus::id_type const &parent_id) {
            parents.push_back(parent_id);
        }

        void add_child(Node *child_virus) {
            std::shared_ptr<Node> pointer_to_child(child_virus);
            children.push_back(pointer_to_child);
        }
    };

    std::map<typename Virus::id_type, Node> viral_map;
    Virus::id_type stem_id;

public:
    //Wydaje mi się, że to nie może być constexpr (chyba sama mapa nie może, więc to tym bardziej)
    VirusGenealogy(typename Virus::id_type const &stem_id) : stem_id(stem_id) {
        viral_map.insert(Node(stem_id));    //wyjątek?
    };

    //Jeśli konstruktor nie może, to chyba wgl nie ma sensu robić rzeczy constexpr
    constexpr Virus::id_type get_stem_id() const {
        return stem_id;                     //wyjątek?
    };

    //Dodałem chwilowo, by mój kompilator się nie pruł, do usunięcia.
    using children_iterator = int;

    // TODO
    // trzeba samemu zdefiniować children_iterator
    constexpr VirusGenealogy<Virus>::children_iterator get_children_begin(typename Virus::id_type const &id) const;
    // TODO
    // trzeba samemu zdefiniować children_iterator
    constexpr VirusGenealogy<Virus>::children_iterator get_children_end(typename Virus::id_type const &id) const;

    // TODO
    std::vector<typename Virus::id_type> get_parents(typename Virus::id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFoundException();
        }
        return viral_map[id].parents; // tutaj można tak czy trzeba przez copy() ?
        //To chyba i tak kopiuje zawartość. Trochę nie jestem jednak pewien czy nie może rzucać wyjątków w trakcie (ale to bym zostawił na potem)
    };

    bool exists(typename Virus::id_type const &id) const {
        return viral_map.contains(id);
        //Czy może tutaj zostać rzucony wyjątek? (np jak operator porównania rzuci) <- to pytanie dotyczy wsm każdej funkcji xd
    };

    const Virus& operator[](typename Virus::id_type const &id) const {
        if (!exists(id)) {
            throw VirusNotFoundException();
        }
        return &viral_map[id];
    };

    void create(typename Virus::id_type const &id, typename Virus::id_type const &parent_id) {
        if (exists(id)) {
            throw VirusAlreadyCreatedException();
        }
        if (!exists(parent_id)) {
            throw VirusNotFoundException();
        }

        viral_map[id] = Node(id);
        viral_map[id].add_parent(parent_id);
        viral_map[parent_id].add_child(&viral_map[id]);
    };

    void create(typename Virus::id_type const &id, std::vector<typename Virus::id_type> const &parent_ids) {
        if (exists(id)) {
            throw VirusAlreadyCreatedException();
        }
        for (auto parent : parent_ids) {
            if (!exists(parent)) {
                throw VirusNotFoundException();
            }
        }

        viral_map[id] = Node(id);
        for (auto parent : parent_ids) {
            viral_map[id].add_parent(parent);
            viral_map[parent].add_child(&viral_map[id]);
        }
    };

    void connect(typename Virus::id_type const &child_id, typename Virus::id_type const &parent_id) {
        if (!exists(child_id) || !exists(parent_id)) {
            throw VirusNotFoundException();
        }
        viral_map[child_id].add_parent(parent_id);
        viral_map[parent_id].add_child(&viral_map[child_id]);
    };

    // TODO
    void remove(typename Virus::id_type const &id) {
        if (!exists(id)) {
            throw VirusNotFoundException();
        }
        if (id == get_stem_id()) {
            throw TriedToRemoveStemVirusException();
        }
        // tu będzie usuwanie jak się dowiemy co ma robić
    };
};

#endif //VIRUS_VIRUS_GENEALOGY_H
