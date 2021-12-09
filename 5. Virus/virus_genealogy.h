#ifndef VIRUS_VIRUS_GENEALOGY_H
#define VIRUS_VIRUS_GENEALOGY_H

#include <vector>
#include <memory>
#include <map>
#include <set>
#include <iterator>

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
        std::set<std::shared_ptr<Node>> children;
        std::set<typename Virus::id_type> parents;

        Node(Virus::id_type const &virus_id) : virus(virus_id) {};

        void add_parent(Virus::id_type const &parent_id) {
            parents.insert(parent_id);
        };

        void add_child(Node *child_virus) {
            std::shared_ptr<Node> pointer_to_child(child_virus);
            children.insert(pointer_to_child);
        };

        Node* create_and_add_child(typename Virus::id_type const &child_id) {
            std::shared_ptr<Node> pointer_to_child = make_shared<Node>(child_id);
            children.insert(pointer_to_child);
            return pointer_to_child.get();
        };
    };

    std::map<typename Virus::id_type, Node*> viral_map;
    std::shared_ptr<Node> stem_node;

public:
    //Wydaje mi się, że to nie może być constexpr (chyba sama mapa nie może, więc to tym bardziej)
    VirusGenealogy(typename Virus::id_type const &stem_id) {
        stem_node = std::shared_ptr<Node>(stem_id);
        viral_map.insert({stem_id, stem_node.get()});    //wyjątek?
    };

    //Jeśli konstruktor nie może, to chyba wgl nie ma sensu robić rzeczy constexpr
    constexpr Virus::id_type get_stem_id() const {
        return stem_node.get()->virus.get_id();
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
        return std::vector<typename Virus::id_type>(viral_map[id].parents); // tutaj można tak czy trzeba przez copy() ?
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
        return viral_map[id];
    };

    void create(typename Virus::id_type const &id, typename Virus::id_type const &parent_id) {
        if (exists(id)) {
            throw VirusAlreadyCreatedException();
        }
        if (!exists(parent_id)) {
            throw VirusNotFoundException();
        }

        viral_map[id]->add_parent(parent_id);
        Node *to_map = viral_map[parent_id]->create_and_add_child(id);
        viral_map[id] = to_map;
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

        create(id, parent_ids[0]);
        for (int i = 1; i < parent_ids.size(); i++) {
            connect(id, parent_ids[i]);
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
    void remove(Virus::id_type const &id) {
        if (!exists(id)) {
            throw VirusNotFoundException();
        }
        if (id == get_stem_id()) {
            throw TriedToRemoveStemVirusException();
        }
        
        try {
            std::vector<typename std::map<typename Virus::id_type, Node>::iterator> to_be_erased;
            //Tę mapę można chyba zamienić na wektor (set jest posortowany), ale utrzymałem w jednej konwencji
            std::map<Node *, typename std::set<typename Virus::id_type> :: iterator> erased_children;
            std::multimap<Node *, typename std::set<typename Virus::id_type>::iterator> erased_parents;
            Node *getting_erased = viral_map.at(id);
            add_erased(to_be_erased, erased_parents, erased_children, id, true);

            //Jeśli tutaj dotarliśmy bez wyjątku to reszta się wykona -> nic tam nie jest rzucane
            for (auto erase_from_map : to_be_erased) {
                viral_map.erase(erase_from_map);
            }
            for (auto parent : erased_parents) {
                parent->first->children.erase(parent->second);
            }
            for (auto child : erased_children) {
                child->first->parents.erase(child->second);
            }
            //Po usunięciu ostatniego shared pointera do usuwanego wierzchołka wymazane zostanie całe poddrzewo
        }
        catch (int e) {
            //Coś rzucić
        }
    };

private: //Tutaj wrzucę jakieś pomocnicze funkcje, do przeniesienia potem

    //Rekurencyjnie dodaje do to_be_erased iteratory do wierzchołków
    // to_be_erased -> iteratory do usunięcia z viral_map
    // erased_children -> mapa, dla każdego ojca (klucz) zapisuje iteratory do miejsc w secie do usunięcia
    // erased_paretns -> iteratory do ojców usuwanego wierzchołka
    void add_erased(std::vector<typename std::map<typename Virus::id_type, Node>::iterator>& to_be_erased,
                    std::multimap<Node *, typename std::set<typename Virus::id_type>::iterator>& erased_parents,
                    std::map<Node *, typename std::set<typename Virus::id_type> :: iterator>& erased_children,
                    Virus::id_type const &id,
                    bool first = false) {
        auto to_erase = viral_map.find(id);
        to_be_erased.add(to_erase);

        for (auto child = to_erase->children.begin(); child != to_erase->children.end(); ++child) {
            auto parent_at_child = child->second.parents.find(id);
            erased_parents.insert({child.get(), parent_at_child});
            if (erased_parents.count(child.get()) == child.get()->parents.size()) {
                add_erased(to_be_erased, erased_parents, id);
            }
        }
        
        if (first)        //Żaden ojciec inny niż ojciec pierwszego usuwanego wierzchołka nie straci dziecka
            for (auto parent = to_erase->parents.begin(); parent != to_erase->parents.end(); ++parent)
                erased_parents.insert({viral_map[*parent], parent->children.find(id)});
    };
};

#endif //VIRUS_VIRUS_GENEALOGY_H
